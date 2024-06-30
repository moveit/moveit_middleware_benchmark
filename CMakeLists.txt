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
find_package(moveit_msgs REQUIRED)
find_package(moveit_servo REQUIRED)
find_package(moveit_core REQUIRED)

add_executable(benchmark_main src/benchmark_main.cpp
                              src/scenarios/scenario_perception_pipeline.cpp)

ament_target_dependencies(
  benchmark_main
  PUBLIC
  "moveit_ros_planning_interface"
  "rclcpp"
  "benchmark"
  "dynmsg"
  "nav_msgs"
  "yaml-cpp")

target_include_directories(
  benchmark_main PUBLIC $<INSTALL_INTERFACE:include>
                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(benchmark_main PUBLIC "benchmark::benchmark"
                                            ${YAML_CPP_LIBRARIES})

add_executable(servo_benchmark_main src/servo_benchmark_main.cpp
                                    src/scenarios/scenario_servo_pipeline.cpp)

ament_target_dependencies(
  servo_benchmark_main
  PUBLIC
  "moveit_ros_planning_interface"
  "rclcpp"
  "benchmark"
  "dynmsg"
  "nav_msgs"
  "yaml-cpp"
  "moveit_msgs"
  "moveit_servo"
  "moveit_core")

target_include_directories(
  servo_benchmark_main
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(servo_benchmark_main PUBLIC "benchmark::benchmark"
                                                  ${YAML_CPP_LIBRARIES})

install(TARGETS benchmark_main servo_benchmark_main
        DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME})

ament_package()
