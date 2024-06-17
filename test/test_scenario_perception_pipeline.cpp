#include <rclcpp/rclcpp.hpp>
#include <benchmark/benchmark.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <memory>

#include <moveit/moveit_cpp/moveit_cpp.h>
#include <moveit/moveit_cpp/planning_component.h>

#include <geometry_msgs/msg/pose.hpp>
#include <nav_msgs/msg/path.hpp>
#include <dynmsg/msg_parser.hpp>
#include <dynmsg/typesupport.hpp>
#include <dynmsg/yaml_utils.hpp>

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <iostream>

using moveit::planning_interface::MoveGroupInterface;

const std::string PLANNING_GROUP = "panda_arm";

class ScenarioPerceptionPipeline
{
public:
  ScenarioPerceptionPipeline(std::shared_ptr<MoveGroupInterface>);
  std::tuple<int, int> runTestCase(const nav_msgs::msg::Path& pose_list);
  bool sendTargetPose(const geometry_msgs::msg::Pose& target_pose);

private:
  std::shared_ptr<MoveGroupInterface> move_group_interface_ptr_;
};

ScenarioPerceptionPipeline::ScenarioPerceptionPipeline(std::shared_ptr<MoveGroupInterface> move_group_interface_ptr)
  : move_group_interface_ptr_(move_group_interface_ptr)
{
}

std::tuple<int, int> ScenarioPerceptionPipeline::runTestCase(const nav_msgs::msg::Path& test_case)
{
  int success_number = 0;
  int failure_number = 0;
  for (auto& pose_stamped : test_case.poses)
  {
    bool is_successful = sendTargetPose(pose_stamped.pose);
    if (is_successful)
      success_number++;
    else
      failure_number++;
  }

  return { success_number, failure_number };
}

bool ScenarioPerceptionPipeline::sendTargetPose(const geometry_msgs::msg::Pose& target_pose)
{
  move_group_interface_ptr_->setPoseTarget(target_pose);

  moveit::planning_interface::MoveGroupInterface::Plan plan_msg;
  const auto ok = static_cast<bool>(move_group_interface_ptr_->plan(plan_msg));

  if (ok)
  {
    move_group_interface_ptr_->execute(plan_msg);
    return true;
  }
  else
  {
    return false;
  }
}

class ScenarioPerceptionPipelineTestCaseCreator
{
private:
  static inline std::vector<nav_msgs::msg::Path> test_cases_ = {};

public:
  static void createTestCases()
  {
    const std::string yaml_file_path = ament_index_cpp::get_package_share_directory("moveit_middleware_benchmark") +
                                       "/config/test_scenario_perception_pipeline.yaml";
    readTestCasesFromFile(yaml_file_path);
  }

  static nav_msgs::msg::Path selectTestCases(size_t test_case_index)
  {
    return test_cases_.at(test_case_index);
  }

  static void readTestCasesFromFile(const std::string& yaml_file_name)
  {
    YAML::Node config = YAML::LoadFile(yaml_file_name.c_str());
    for (YAML::const_iterator it = config["test_cases"].begin(); it != config["test_cases"].end(); ++it)
    {
      const std::string yaml_string = dynmsg::yaml_to_string(*it);
      nav_msgs::msg::Path test_case = getTestCaseFromYamlString(yaml_string);

      test_cases_.push_back(test_case);
    }
  }

  static nav_msgs::msg::Path getTestCaseFromYamlString(const std::string& yaml_string)
  {
    nav_msgs::msg::Path path_msg;
    void* ros_message = reinterpret_cast<void*>(&path_msg);

    dynmsg::cpp::yaml_and_typeinfo_to_rosmsg(dynmsg::cpp::get_type_info({ "nav_msgs", "Path" }), yaml_string,
                                             ros_message);

    return path_msg;
  }
};

class ScenarioPerceptionPipelineFixture : public benchmark::Fixture
{
protected:
  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<MoveGroupInterface> move_group_interface_ptr_;
  size_t selected_test_case_index_;

public:
  ScenarioPerceptionPipelineFixture()
  {
    ScenarioPerceptionPipelineTestCaseCreator::createTestCases();
  }

  void SetUp(::benchmark::State& /*state*/)
  {
    if (node_.use_count() == 0)
    {
      node_ =
          std::make_shared<rclcpp::Node>("test_scenario_perception_pipeline_node",
                                         rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

      node_->get_parameter("selected_test_case_index", selected_test_case_index_);
    }

    if (move_group_interface_ptr_.use_count() == 0)
    {
      move_group_interface_ptr_ = std::make_shared<MoveGroupInterface>(node_, PLANNING_GROUP);
    }
  }

  void TearDown(::benchmark::State& /*state*/)
  {
  }
};

BENCHMARK_DEFINE_F(ScenarioPerceptionPipelineFixture, test_scenario_perception_pipeline)(benchmark::State& st)
{
  auto selected_test_case = ScenarioPerceptionPipelineTestCaseCreator::selectTestCases(selected_test_case_index_);
  for (auto _ : st)
  {
    auto sc = ScenarioPerceptionPipeline(move_group_interface_ptr_);
    auto [success_number, failure_number] = sc.runTestCase(selected_test_case);
    st.counters["success_number"] = success_number;
    st.counters["failure_number"] = failure_number;
  }
}

BENCHMARK_REGISTER_F(ScenarioPerceptionPipelineFixture, test_scenario_perception_pipeline);

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  rclcpp::shutdown();
  return 0;
}
