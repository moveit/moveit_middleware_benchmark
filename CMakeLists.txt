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

add_executable(
  scenario_perception_pipeline_benchmark_main
  src/scenario_perception_pipeline_benchmark_main.cpp
  src/scenarios/scenario_perception_pipeline.cpp)

ament_target_dependencies(
  scenario_perception_pipeline_benchmark_main
  PUBLIC
  "moveit_ros_planning_interface"
  "rclcpp"
  "benchmark"
  "dynmsg"
  "nav_msgs"
  "yaml-cpp")

target_include_directories(
  scenario_perception_pipeline_benchmark_main
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(scenario_perception_pipeline_benchmark_main
                      PUBLIC "benchmark::benchmark" ${YAML_CPP_LIBRARIES})

install(TARGETS scenario_perception_pipeline_benchmark_main
        DESTINATION lib/moveit_middleware_benchmark)

install(DIRECTORY launch config DESTINATION share/moveit_middleware_benchmark)

ament_package()
