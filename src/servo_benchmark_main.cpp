#include "moveit_middleware_benchmark/scenarios/scenario_servo_pipeline.hpp"

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  rclcpp::shutdown();
  return 0;
}
