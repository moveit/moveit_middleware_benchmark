## How To Run Basic Service Client Benchmark

Firstly, source your ros version. It's suggested to test with rolling version of ROS.

For instance, to test with rmw_zenoh, start to zenoh router using following command in the terminal.
```sh
# go to your workspace
cd ws
# Be sure that ros2 daemon is killed.
pkill -9 -f ros && ros2 daemon stop
# Then start zenoh router
source /opt/ros/rolling/setup.bash
source install/setup.bash
export RMW_IMPLEMENTATION=rmw_zenoh_cpp
ros2 run rmw_zenoh_cpp rmw_zenohd
```

Select your rmw_implementation as `rmw_zenoh_cpp` and run the perception benchmark launch file in the another terminal.
```sh
# go to your workspace
cd ws
source /opt/ros/rolling/setup.bash
source install/setup.bash
export RMW_IMPLEMENTATION=rmw_zenoh_cpp # select your rmw_implementation to benchmark
ros2 launch moveit_middleware_benchmark scenario_basic_service_client_benchmark.launch.py
```

It will be defaultly benchmarked with 6 repetitions. It will be created the json file named `middleware_benchmark_results.json` for benchmarking results after finishing benchmark code execution. You can see the benchmark results in more detail inside this json file.

If you want to customize your benchmark arguments or select different test case, you can use below command.

```shell
ros2 launch moveit_middleware_benchmark scenario_perception_pipeline_benchmark.launch.py benchmark_command_args:="--benchmark_out=middleware_benchmark_results.json --benchmark_out_format=json --benchmark_repetitions=1" sending_request_number:=50000
```

## How to benchmark the basic service client execution

The main idea here is to send some client request in `example_interfaces::srv::AddTwoInts` format to `add_two_ints_server` which is one of the examples of [ros2/demos package](https://github.com/ros2/demos) and then to measure the elapsed time by waiting response from server. This logic helps us to measure elapsed time between sending request and receiving response.
