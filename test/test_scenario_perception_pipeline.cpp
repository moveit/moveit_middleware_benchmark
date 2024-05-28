#include <rclcpp/rclcpp.hpp>
#include <benchmark/benchmark.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <memory>

#include <moveit/moveit_cpp/moveit_cpp.h>
#include <moveit/moveit_cpp/planning_component.h>

#include <geometry_msgs/msg/pose.hpp>

using moveit::planning_interface::MoveGroupInterface;

// class MoveGroupServer {
// };

class ScenarioPerceptionPipeline {
public:
    ScenarioPerceptionPipeline(std::shared_ptr<MoveGroupInterface>);
    bool go_to_poses(std::vector<geometry_msgs::msg::Pose> pose_list);
private:
    std::shared_ptr<MoveGroupInterface> move_group_interface_ptr_;
};

ScenarioPerceptionPipeline::ScenarioPerceptionPipeline(std::shared_ptr<MoveGroupInterface> move_group_interface_ptr) 
: move_group_interface_ptr_(move_group_interface_ptr) { }

bool ScenarioPerceptionPipeline::go_to_poses(std::vector<geometry_msgs::msg::Pose> pose_list) {
    for (auto& pose : pose_list) {
        move_group_interface_ptr_->setPoseTarget(pose);

        moveit::planning_interface::MoveGroupInterface::Plan plan_msg;
        auto const ok = static_cast<bool>(move_group_interface_ptr_->plan(plan_msg));

        if (ok) {
            move_group_interface_ptr_->execute(plan_msg);
        } else {
            return false;
        }
    }

    return true;
}

class ScenarioPerceptionPipelineFixture : public benchmark::Fixture {
public:
    std::vector<geometry_msgs::msg::Pose> example_pose_list_;
    std::shared_ptr<rclcpp::Node> node_;
    std::shared_ptr<MoveGroupInterface> move_group_interface_ptr_;

    void SetUp(::benchmark::State & state) {
        geometry_msgs::msg::Pose target_pose1_msg;
        target_pose1_msg.orientation.w = 1.0;
        target_pose1_msg.position.x = -0.7;
        target_pose1_msg.position.y = 0.0;
        target_pose1_msg.position.z = 0.5;

        geometry_msgs::msg::Pose target_pose2_msg;
        target_pose2_msg.orientation.w = 1.0;
        target_pose2_msg.position.x = 0.4;
        target_pose2_msg.position.y = 0.0;
        target_pose2_msg.position.z = 0.7;

        for (int i = 0; i < 10; i++) {
            example_pose_list_.push_back(target_pose1_msg);
            example_pose_list_.push_back(target_pose2_msg);
        }

        if (node_.use_count() == 0) {
            node_ = std::make_shared<rclcpp::Node>(
        "test_scenario_perception_pipeline_node", rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true)
    );
        }

        if (move_group_interface_ptr_.use_count() == 0) {
            move_group_interface_ptr_ = std::make_shared<MoveGroupInterface>(node_, "panda_arm");
        }
    }

    void TearDown(::benchmark::State & state) {
    }
};

BENCHMARK_DEFINE_F(ScenarioPerceptionPipelineFixture, elma)(benchmark::State & st) {
    for (auto _ : st) {
        auto sc = ScenarioPerceptionPipeline(move_group_interface_ptr_);
        sc.go_to_poses(example_pose_list_);
    }
}

BENCHMARK_REGISTER_F(ScenarioPerceptionPipelineFixture, elma);

int main(int argc, char ** argv) {
    rclcpp::init(argc, argv);
    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
    rclcpp::shutdown();
    return 0;
}