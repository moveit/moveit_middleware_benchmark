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

add_executable(test_scenario_perception_pipeline
               test/test_scenario_perception_pipeline.cpp)

ament_target_dependencies(
  test_scenario_perception_pipeline
  PUBLIC
  "moveit_ros_planning_interface"
  "rclcpp"
  "benchmark"
  "dynmsg"
  "nav_msgs")

target_include_directories(
  test_scenario_perception_pipeline
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(test_scenario_perception_pipeline
                      PUBLIC "benchmark::benchmark")

install(TARGETS test_scenario_perception_pipeline
        DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME})

ament_package()
