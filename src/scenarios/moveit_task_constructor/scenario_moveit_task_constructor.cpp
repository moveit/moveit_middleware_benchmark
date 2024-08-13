#include "moveit_middleware_benchmark/scenarios/moveit_task_constructor/scenario_moveit_task_constructor.hpp"

namespace moveit {
namespace middleware_benchmark {

ScenarioMoveItTaskConstructor::ScenarioMoveItTaskConstructor(const rclcpp::Node::SharedPtr & node) : node_(node) {

    pick_place_task_param_listener_ = std::make_shared<pick_place_task_demo::ParamListener>(node_);
	pick_place_task_demo_parameters_ = pick_place_task_param_listener_->get_params();

	moveit_task_constructor_demo::setupDemoScene(pick_place_task_demo_parameters_);
    pick_place_task_ = std::make_shared<moveit_task_constructor_demo::PickPlaceTask>("pick_place_task");

    if (!pick_place_task_->init(node_, pick_place_task_demo_parameters_)) {
		RCLCPP_INFO(node->get_logger(), "Initialization failed");
	}
}

void ScenarioMoveItTaskConstructor::runTestCase() {
    if (pick_place_task_->plan(10)) {
        pick_place_task_->execute();
		RCLCPP_INFO(node_->get_logger(), "Planning succeded");
	} else {
		RCLCPP_ERROR(node_->get_logger(), "Planning failed");
	}
}

ScenarioMoveItTaskConstructor::~ScenarioMoveItTaskConstructor() {
    moveit_task_constructor_demo::destroyDemoScene(pick_place_task_demo_parameters_);
}

ScenarioMoveItTaskConstructorFixture::ScenarioMoveItTaskConstructorFixture() {

}

void ScenarioMoveItTaskConstructorFixture::SetUp(::benchmark::State& /*state*/) {
  if (node_.use_count() == 0)
  {
    node_ = std::make_shared<rclcpp::Node>("test_scenario_moveit_task_constructor_node",
                                           rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

    node_executor_ = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
    node_executor_->add_node(node_);

    spinning_thread_ = std::make_unique<std::thread>([this]() {node_executor_->spin();});
  }
}

void ScenarioMoveItTaskConstructorFixture::TearDown(::benchmark::State& /*state*/) {
  node_executor_->cancel();

  if (spinning_thread_->joinable()) {
    spinning_thread_->join();
  }

  node_executor_.reset();
  node_.reset();
}

BENCHMARK_DEFINE_F(ScenarioMoveItTaskConstructorFixture, test_scenario_moveit_task_constructor)(benchmark::State& st)
{
  for (auto _ : st)
  {
    auto sc = ScenarioMoveItTaskConstructor(node_);
    sc.runTestCase();
  }
}

BENCHMARK_REGISTER_F(ScenarioMoveItTaskConstructorFixture, test_scenario_moveit_task_constructor);

}
}