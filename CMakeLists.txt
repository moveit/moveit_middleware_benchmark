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

add_subdirectory(src/scenario_basic_service_client)
add_subdirectory(src/scenario_perception_pipeline)

install(DIRECTORY launch config DESTINATION share/moveit_middleware_benchmark)

ament_package()
