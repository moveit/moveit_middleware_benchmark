#include "moveit_middleware_benchmark/scenarios/scenario_servo_pipeline.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <iostream>

namespace moveit
{
namespace middleware_benchmark
{

ProcessUtils::ProcessUtils(rclcpp::Node::SharedPtr node) : node_(node)
{
}

void ProcessUtils::startROSControllers()
{
  pid_t pid = fork();
  if (pid == 0)
  {
    // child process
    // TODO @CihatAltiparmak : Move this command arguments into parameter file
    const std::string PANDA_PACKAGE_SHARE_DIRECTORY =
        ament_index_cpp::get_package_share_directory("moveit_resources_panda_moveit_config");

    const std::string ros2_control_param_file = PANDA_PACKAGE_SHARE_DIRECTORY + "/config/ros2_controllers.yaml";
    char* ros2_control_param_file_cstr = new char[ros2_control_param_file.length() + 1];
    std::strcpy(ros2_control_param_file_cstr, ros2_control_param_file.c_str());

    char* command[] = { "/opt/ros/rolling/lib/controller_manager/ros2_control_node",
                        "--ros-args",
                        "--params-file",
                        ros2_control_param_file_cstr,
                        "--remap",
                        "/controller_manager/robot_description:=/robot_description",
                        NULL };

    execvp("/opt/ros/rolling/lib/controller_manager/ros2_control_node", command);

    RCLCPP_FATAL(node_->get_logger(), "A problem occurred while running controller manager node in child node!");
    exit(1);
  }
  else if (pid > 0)
  {
    ros2_controller_pid_ = pid;

    // TODO @CihatAltiparmak : Move this command arguments into parameter file
    std::system("ros2 run controller_manager spawner joint_state_broadcaster --controller-manager-timeout 3000 "
                "--controller-manager /controller_manager");
    std::system("ros2 run controller_manager spawner panda_arm_controller --controller-manager-timeout 3000 "
                "--controller-manager /controller_manager");
  }
  else
  {
    RCLCPP_FATAL(node_->get_logger(),
                 "A problem occurred while creating a child process in order to run controller manager!");
    exit(1);
  }
}

void ProcessUtils::killROSControllers()
{
  RCLCPP_INFO(
      node_->get_logger(),
      "ProcessUtils::killROSControllers() started to kill the child process controller_manager is run with pid %d",
      ros2_controller_pid_);
  kill(ros2_controller_pid_, SIGTERM);
  int status;
  std::cout << "[ProcessUtils::killROSControllers] [ waiting for ] " << ros2_controller_pid_ << std::endl;
  waitpid(ros2_controller_pid_, &status, 0);
  RCLCPP_INFO(node_->get_logger(),
              "ProcessUtils::killROSControllers() waiting the process with %d pid is finished! Got status : %d",
              ros2_controller_pid_, status);
}

ScenarioServoPipeline::ScenarioServoPipeline(rclcpp::Node::SharedPtr node, const rclcpp::Duration& server_timeout)
  : node_(node), server_timeout_(server_timeout), latest_servo_status_code_(moveit_servo::StatusCode::NO_WARNING)
{
  node_->get_parameter("selected_link", tested_link_);
  ;

  servo_switch_command_type_client_ =
      node_->create_client<moveit_msgs::srv::ServoCommandType>("/servo_node/switch_command_type");
  if (servo_switch_command_type_client_->wait_for_service(server_timeout_.to_chrono<std::chrono::duration<double>>()))
  {
    RCLCPP_INFO(node_->get_logger(), "SwitchCommandType Server is ready!");
  }
  else
  {
    RCLCPP_ERROR(node_->get_logger(), "SwitchCommandType Server is not ready!");
  }

  servo_command_pubisher_ = node_->create_publisher<geometry_msgs::msg::TwistStamped>("/servo_node/delta_twist_cmds",
                                                                                      rclcpp::SystemDefaultsQoS());

  servo_status_executor_ = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
  auto callback_group = node_->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive,
                                                     false /* don't spin with node executor */);
  auto options = rclcpp::SubscriptionOptions();
  options.callback_group = callback_group;
  servo_status_subscriber_ = node_->create_subscription<moveit_msgs::msg::ServoStatus>(
      "/servo_node/status", rclcpp::SystemDefaultsQoS(),
      [this](moveit_msgs::msg::ServoStatus::SharedPtr new_servo_status_msg) {
        latest_servo_status_code_ = static_cast<moveit_servo::StatusCode>(new_servo_status_msg->code);
      },
      options);

  servo_status_executor_->add_callback_group(callback_group, node_->get_node_base_interface());
  servo_status_executor_thread_ = std::thread([this]() { servo_status_executor_->spin(); });
}

ScenarioServoPipeline::~ScenarioServoPipeline()
{
  if (servo_status_executor_)
  {
    servo_status_executor_->cancel();
  }

  if (servo_status_executor_thread_.joinable())
  {
    servo_status_executor_thread_.join();
  }
}

void ScenarioServoPipeline::runTestCase(const geometry_msgs::msg::Twist& test_case)
{
  rclcpp::WallRate servo_frequency(10);
  while (getServoStatus() == moveit_servo::StatusCode::NO_WARNING)
  {
    this->sendTwistCommandToServo(tested_link_, test_case);
    servo_frequency.sleep();
  }
}

void ScenarioServoPipeline::switchCommandType(const moveit_servo::CommandType& servo_command_type,
                                              const rclcpp::Duration& timeout)
{
  auto servo_command_type_request = std::make_shared<moveit_msgs::srv::ServoCommandType::Request>();
  servo_command_type_request->command_type = static_cast<int8_t>(servo_command_type);

  auto result = servo_switch_command_type_client_->async_send_request(servo_command_type_request);
  if (rclcpp::spin_until_future_complete(node_, result, timeout.to_chrono<std::chrono::duration<double>>()) ==
      rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_INFO(node_->get_logger(), "Got the response from servo switch command server!");
  }
  else
  {
    RCLCPP_ERROR(node_->get_logger(), "A problem occurred while sending switch command to servo node!");
  }
}

moveit_servo::StatusCode ScenarioServoPipeline::getServoStatus()
{
  return latest_servo_status_code_;
}

void ScenarioServoPipeline::sendTwistCommandToServo(const std::string& link,
                                                    const geometry_msgs::msg::Twist& target_twist)
{
  geometry_msgs::msg::TwistStamped target_twist_stamped;
  target_twist_stamped.twist = target_twist;
  target_twist_stamped.header.stamp = node_->now();
  target_twist_stamped.header.frame_id = link;

  servo_command_pubisher_->publish(target_twist_stamped);
}

ScenarioServoPipelineFixture::ScenarioServoPipelineFixture()
{
  createTestCases();
}

void ScenarioServoPipelineFixture::SetUp(benchmark::State& /*state*/)
{
  process_utils_.startROSControllers();

  if (node_.use_count() == 0)
  {
    node_ = std::make_shared<rclcpp::Node>("test_servo_pipeline_node",
                                           rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));
    // TODO @CihatAltiparmak : assign initial value for ros arguments
    // for example, selected_test_case_index_ = 0;
    node_->get_parameter("selected_test_case_index", selected_test_case_index_);
    RCLCPP_INFO(node_->get_logger(), "SELECTED TEST CASE INDEX: %ld", selected_test_case_index_);

    process_utils_ = ProcessUtils(node_);
  }
}

void ScenarioServoPipelineFixture::TearDown(benchmark::State& /*state*/)
{
  process_utils_.killROSControllers();
  RCLCPP_INFO(node_->get_logger(), "ScenarioServoPipelineFixture::TearDown() ROS 2 Controller is terminated!");
  std::cout << "ROS 2 Controller is terminated!" << std::endl;
}

geometry_msgs::msg::Twist ScenarioServoPipelineFixture::selectTestCase(size_t test_case_index)
{
  return test_cases_.at(test_case_index);
}

void ScenarioServoPipelineFixture::createTestCases()
{
  readTestCasesFromFile(TEST_CASES_YAML_FILE);
}

void ScenarioServoPipelineFixture::readTestCasesFromFile(const std::string& yaml_file_name)
{
  YAML::Node config = YAML::LoadFile(yaml_file_name.c_str());
  for (YAML::const_iterator it = config["test_cases"].begin(); it != config["test_cases"].end(); ++it)
  {
    const std::string yaml_string = dynmsg::yaml_to_string(*it);
    geometry_msgs::msg::Twist test_case = getTestCaseFromYamlString(yaml_string);

    test_cases_.push_back(test_case);
  }
}

geometry_msgs::msg::Twist ScenarioServoPipelineFixture::getTestCaseFromYamlString(const std::string& yaml_string)
{
  geometry_msgs::msg::Twist twist_msg;
  void* ros_message = reinterpret_cast<void*>(&twist_msg);

  dynmsg::cpp::yaml_and_typeinfo_to_rosmsg(dynmsg::cpp::get_type_info({ "geometry_msgs", "Twist" }), yaml_string,
                                           ros_message);

  return twist_msg;
}

BENCHMARK_DEFINE_F(ScenarioServoPipelineFixture, test_scenario_servo_pipeline)(::benchmark::State& st)
{
  auto selected_test_case = selectTestCase(selected_test_case_index_);

  servo_pipeline_ = std::make_shared<moveit::middleware_benchmark::ScenarioServoPipeline>(node_);
  servo_pipeline_->switchCommandType(moveit_servo::CommandType::TWIST);

  for (auto _ : st)
  {
    servo_pipeline_->runTestCase(selected_test_case);
  }
}

BENCHMARK_REGISTER_F(ScenarioServoPipelineFixture, test_scenario_servo_pipeline)->MeasureProcessCPUTime()->UseRealTime();

}  // namespace middleware_benchmark
}  // namespace moveit
