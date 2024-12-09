add_executable(scenario_perception_pipeline_benchmark_main
               scenario_perception_pipeline_benchmark_main.cpp
               scenario_perception_pipeline.cpp)

ament_target_dependencies(
  scenario_perception_pipeline_benchmark_main
  PUBLIC
  moveit_ros_planning_interface
  rclcpp
  benchmark
  dynmsg
  nav_msgs
  yaml-cpp
  example_interfaces)

target_include_directories(
  scenario_perception_pipeline_benchmark_main
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)

target_link_libraries(scenario_perception_pipeline_benchmark_main
                      PUBLIC benchmark::benchmark ${YAML_CPP_LIBRARIES})

install(TARGETS scenario_perception_pipeline_benchmark_main
        DESTINATION lib/moveit_middleware_benchmark)