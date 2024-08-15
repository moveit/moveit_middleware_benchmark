cmake_minimum_required(VERSION 3.8)
project(moveit_middleware_benchmark)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

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
find_package(moveit_task_constructor_core REQUIRED)
find_package(generate_parameter_library REQUIRED)

add_executable(
  scenario_perception_pipeline_benchmark_main
  src/scenario_perception_pipeline_benchmark_main.cpp
  src/scenarios/scenario_perception_pipeline.cpp)

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
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(scenario_perception_pipeline_benchmark_main
                      PUBLIC benchmark::benchmark ${YAML_CPP_LIBRARIES})

add_executable(
  scenario_basic_service_client_benchmark_main
  src/scenario_basic_service_client_benchmark_main.cpp
  src/scenarios/scenario_basic_service_client.cpp)

ament_target_dependencies(
  scenario_basic_service_client_benchmark_main
  PUBLIC
  moveit_ros_planning_interface
  rclcpp
  benchmark
  std_msgs
  example_interfaces)

target_include_directories(
  scenario_basic_service_client_benchmark_main
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(scenario_basic_service_client_benchmark_main
                      PUBLIC "benchmark::benchmark" ${YAML_CPP_LIBRARIES})

add_executable(
  scenario_moveit_task_constructor_benchmark_main
  src/scenario_moveit_task_constructor_benchmark_main.cpp
  src/scenarios/moveit_task_constructor/scenario_moveit_task_constructor.cpp
  src/scenarios/moveit_task_constructor/pick_place_task.cpp)

ament_target_dependencies(
  scenario_moveit_task_constructor_benchmark_main
  PUBLIC
  moveit_ros_planning_interface
  rclcpp
  benchmark
  std_msgs
  example_interfaces
  moveit_task_constructor_core)

target_include_directories(
  scenario_moveit_task_constructor_benchmark_main
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

generate_parameter_library(
  pick_place_demo_parameters
  src/scenarios/moveit_task_constructor/pick_place_demo_parameters.yaml)

target_link_libraries(
  scenario_moveit_task_constructor_benchmark_main
  PUBLIC benchmark::benchmark ${YAML_CPP_LIBRARIES}
         pick_place_demo_parameters)

install(
  TARGETS scenario_perception_pipeline_benchmark_main
          scenario_basic_service_client_benchmark_main
          scenario_moveit_task_constructor_benchmark_main
  DESTINATION lib/moveit_middleware_benchmark)

install(DIRECTORY launch config DESTINATION share/moveit_middleware_benchmark)

ament_package()
