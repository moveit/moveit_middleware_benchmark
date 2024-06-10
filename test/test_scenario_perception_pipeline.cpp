#include <rclcpp/rclcpp.hpp>
#include <benchmark/benchmark.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <memory>

#include <moveit/moveit_cpp/moveit_cpp.h>
#include <moveit/moveit_cpp/planning_component.h>

#include <geometry_msgs/msg/pose.hpp>

using moveit::planning_interface::MoveGroupInterface;

class ScenarioPerceptionPipeline
{
public:
  ScenarioPerceptionPipeline(std::shared_ptr<MoveGroupInterface>);
  std::tuple<int, int> runTestCase(const std::vector<geometry_msgs::msg::Pose>& pose_list);
  bool sendTargetPose(const geometry_msgs::msg::Pose& target_pose);

private:
  std::shared_ptr<MoveGroupInterface> move_group_interface_ptr_;
};

ScenarioPerceptionPipeline::ScenarioPerceptionPipeline(std::shared_ptr<MoveGroupInterface> move_group_interface_ptr)
  : move_group_interface_ptr_(move_group_interface_ptr)
{
}

std::tuple<int, int> ScenarioPerceptionPipeline::runTestCase(const std::vector<geometry_msgs::msg::Pose>& test_case)
{
  int success_number = 0;
  int failure_number = 0;
  for (auto& pose : test_case)
  {
    bool is_successful = sendTargetPose(pose);
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
  static inline std::vector<std::vector<geometry_msgs::msg::Pose>> test_cases_ = {};

public:
  static void createTestCases()
  {
    std::vector<geometry_msgs::msg::Pose> example_pose_list_;

    geometry_msgs::msg::Pose target_pose1_msg;
    target_pose1_msg.orientation.w = 1.0;
    target_pose1_msg.position.x = 0.5;
    target_pose1_msg.position.y = 0.5;
    target_pose1_msg.position.z = 0.5;

    geometry_msgs::msg::Pose target_pose2_msg;
    target_pose2_msg.orientation.w = 1.0;
    target_pose2_msg.position.x = 0.5;
    target_pose2_msg.position.y = -0.5;
    target_pose2_msg.position.z = 0.7;

    for (int i = 0; i < 10; i++)
    {
      example_pose_list_.push_back(target_pose1_msg);
      example_pose_list_.push_back(target_pose2_msg);
    }

    test_cases_.push_back(example_pose_list_);
  }

  static std::vector<geometry_msgs::msg::Pose> selectTestCases(size_t test_case_index)
  {
    return test_cases_.at(test_case_index);
  }
};

class ScenarioPerceptionPipelineFixture : public benchmark::Fixture
{
protected:
  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<MoveGroupInterface> move_group_interface_ptr_;
  std::string PLANNING_GROUP;

public:
  ScenarioPerceptionPipelineFixture()
  {
    PLANNING_GROUP = "panda_arm";
    ScenarioPerceptionPipelineTestCaseCreator::createTestCases();
  }

  void SetUp(::benchmark::State& /*state*/)
  {
    if (node_.use_count() == 0)
    {
      node_ =
          std::make_shared<rclcpp::Node>("test_scenario_perception_pipeline_node",
                                         rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));
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
  auto selected_test_case = ScenarioPerceptionPipelineTestCaseCreator::selectTestCases(0);
  for (auto _ : st)
  {
    auto sc = ScenarioPerceptionPipeline(move_group_interface_ptr_);
    sc.runTestCase(selected_test_case);
  }
}

BENCHMARK_REGISTER_F(ScenarioPerceptionPipelineFixture, test_scenario_perception_pipeline);

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::Shutdown();
  rclcpp::shutdown();
  return 0;
}
