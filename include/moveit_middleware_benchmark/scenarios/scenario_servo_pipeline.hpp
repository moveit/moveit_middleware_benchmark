#pragma once

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <moveit_msgs/srv/servo_command_type.hpp>
#include <moveit_msgs/msg/servo_status.hpp>
#include <moveit_servo/utils/datatypes.hpp>
#include <thread>
#include <chrono>
#include <memory>
#include <benchmark/benchmark.h>
#include <thread>

#include <dynmsg/msg_parser.hpp>
#include <dynmsg/typesupport.hpp>
#include <dynmsg/yaml_utils.hpp>

#include <ament_index_cpp/get_package_share_directory.hpp>

#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/robot_model/robot_model.h>
#include <moveit/robot_state/robot_state.h>
#include <moveit/planning_scene_monitor/planning_scene_monitor.h>
#include <moveit/planning_scene/planning_scene.h>

namespace
{
const std::string PLANNING_GROUP = "panda_arm";
const std::string PACKAGE_SHARE_DIRECTORY = ament_index_cpp::get_package_share_directory("moveit_middleware_benchmark");
const std::string TEST_CASES_YAML_FILE =
    PACKAGE_SHARE_DIRECTORY + "/config/scenario_servo_pipeline_test_cases.yaml";

}  // namespace

namespace moveit {

namespace middleware_benchmark {

class ScenarioServoPipeline {
public:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Client<moveit_msgs::srv::ServoCommandType>::SharedPtr servo_switch_command_type_client_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr servo_command_pubisher_;
    rclcpp::Subscription<moveit_msgs::msg::ServoStatus>::SharedPtr servo_status_subscriber_;
    std::shared_ptr<rclcpp::executors::SingleThreadedExecutor> servo_status_executor_;
    std::thread servo_status_executor_thread_;
    rclcpp::Duration server_timeout_;
    // std::shared_ptr<planning_scene_monitor::PlanningSceneMonitor> planning_scene_monitor_;
    planning_scene_monitor::PlanningSceneMonitorPtr planning_scene_monitor_;

    ScenarioServoPipeline(rclcpp::Node::SharedPtr node, const rclcpp::Duration & server_timeout = rclcpp::Duration::from_seconds(-1));
    ~ScenarioServoPipeline();
    void switchCommandType(
        const moveit_servo::CommandType & servo_command_type, 
        const rclcpp::Duration & timeout = rclcpp::Duration::from_seconds(-1));
    void sendTwistCommandToServo(const std::string & frame_id, const geometry_msgs::msg::Twist & target_twist);
    moveit_servo::StatusCode getServoStatus();
    void runTestCase(const geometry_msgs::msg::Twist & test_case);

private:
  moveit_servo::StatusCode latest_servo_status_code_;
};

class ScenarioServoPipelineFixture : public benchmark::Fixture {
protected:
    rclcpp::Node::SharedPtr node_;
    std::shared_ptr<moveit::middleware_benchmark::ScenarioServoPipeline> servo_pipeline_;

    /* selected test case index for benchmarking */
    size_t selected_test_case_index_;

    /** the list of test cases to be tested */
    std::vector<geometry_msgs::msg::Twist> test_cases_ = {};
public:

    ScenarioServoPipelineFixture();
    void SetUp(benchmark::State& /*state*/);
    void TearDown(benchmark::State & /*state*/);

    geometry_msgs::msg::Twist selectTestCase(size_t test_case_index);
    void createTestCases();
    void readTestCasesFromFile(const std::string & yaml_file_name);
    geometry_msgs::msg::Twist getTestCaseFromYamlString(const std::string& yaml_string);
};


}
}