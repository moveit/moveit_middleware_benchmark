add_executable(
  scenario_basic_service_client_benchmark_main
  scenario_basic_service_client_benchmark_main.cpp
  scenario_basic_service_client.cpp)

ament_target_dependencies(
  scenario_basic_service_client_benchmark_main
  PUBLIC
  rclcpp
  benchmark
  std_msgs
  example_interfaces
  ament_index_cpp)

target_include_directories(
  scenario_basic_service_client_benchmark_main
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)

target_link_libraries(scenario_basic_service_client_benchmark_main
                      PUBLIC benchmark::benchmark ${YAML_CPP_LIBRARIES})

install(TARGETS scenario_basic_service_client_benchmark_main
        DESTINATION lib/moveit_middleware_benchmark)
