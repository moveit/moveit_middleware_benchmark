## Scenarios

### [Perception Pipeline Benchmark](scenarios/perception_pipeline_benchmark.md)

This benchmark measures the elapsed time by which the determined path is sent for the robot to follow. This benchmark calculates `elapsed time`, `success_number` and `failure_number`. `elapsed_time` is used for measuring how much time the pipeline takes. `success_number` is used for presenting successfully plannings and `failure_number` is used for presenting failed plannings.

Firstly, `node` and `move_group_interface`in SetUp are created before each benchmark. `poses` inside `nav_msgs/msg/Path` is sent one by one to plan trajectory for robot. If planning is failed, only `failure_rate` is increased. If planning is successful, the trajectory_plan which move_group_server plan is sent via `move_group_interface` to start the execution of this planned trajectory. Then `success_number` is increased.

For instance, the selected test_case includes 20 goal poses. These 20 goals is sent one by one to `move_group_server`. If the 5 goal poses out of 20 goal poses are failed, `success_number` equals 15 and `failure_number` equals 5. `success_number` and `failure_number` is important to observe the middlewares' behaviours.

### [Basic Service Client Works Benchmark](scenarios/basic_service_client_benchmark.md)

This benchmark measures the total elapsed time based on the time interval between sending the request by the client to the server and getting the response of server. This benchmark utilizes the [ros2/demos](https://github.com/ros2/demos) packages' [example server](https://github.com/ros2/demos/blob/rolling/demo_nodes_cpp/src/services/add_two_ints_server.cpp).

In this benchmark scenario, the benchmarker node only has client interface. THe necessary server for this client is run in [the launch file of this benchmark scenario](../launch/scenario_basic_service_client_benchmark.launch.py). Client sends a request to server and waits for the response from server. Client sends second request to server once the client receives response of first request from client. This actions are repeated `sending_request_number` times. You can configure this `sending_request_number` parameter in [this scenario's launch file]((../launch/scenario_basic_service_client_benchmark.launch.py)).
