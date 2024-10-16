cmake_minimum_required(VERSION 3.8)
project(moveit_middleware_benchmark)

# Common cmake code applied to all moveit packages
find_package(moveit_common REQUIRED)
moveit_package()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)
find_package(rclcpp REQUIRED)
find_package(benchmark REQUIRED)
find_package(dynmsg REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(example_interfaces REQUIRED)

add_library(
  moveit_middleware_benchmark_lib SHARED
  src/scenarios/scenario_perception_pipeline.cpp
  src/scenarios/scenario_basic_service_client.cpp)

ament_target_dependencies(
  moveit_middleware_benchmark_lib
  PUBLIC
  moveit_ros_planning_interface
  rclcpp
  benchmark
  dynmsg
  nav_msgs
  std_msgs
  yaml-cpp
  example_interfaces)

target_include_directories(
  moveit_middleware_benchmark_lib
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(moveit_middleware_benchmark_lib
                      PUBLIC benchmark::benchmark ${YAML_CPP_LIBRARIES})

add_executable(scenario_perception_pipeline_benchmark_main
               src/scenario_perception_pipeline_benchmark_main.cpp)

target_link_libraries(scenario_perception_pipeline_benchmark_main
                      PUBLIC moveit_middleware_benchmark_lib)

add_executable(scenario_basic_service_client_benchmark_main
               src/scenario_basic_service_client_benchmark_main.cpp)

target_link_libraries(scenario_basic_service_client_benchmark_main
                      PUBLIC moveit_middleware_benchmark_lib)

install(TARGETS scenario_perception_pipeline_benchmark_main
                scenario_basic_service_client_benchmark_main
        DESTINATION lib/moveit_middleware_benchmark)

install(DIRECTORY launch config DESTINATION share/moveit_middleware_benchmark)

ament_package()
