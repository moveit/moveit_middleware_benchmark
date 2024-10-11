## How To Run Basic Topic Subscription Publishing Benchmark

Firstly, source your ros version. It's suggested to test with rolling version of ROS 2.

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
ros2 launch moveit_middleware_benchmark scenario_basic_subscription_benchmark.launch.py
```

It will be defaultly benchmarked with 6 repetitions. It will be created the json file named `middleware_benchmark_results.json` for benchmarking results after finishing benchmark code execution. You can see the benchmark results in more detail inside this json file.

If you want to customize your benchmark arguments or select different test case, you can use below command.

```shell
ros2 launch moveit_middleware_benchmark scenario_basic_subscription_benchmark.launch.py benchmark_command_args:="--benchmark_out=middleware_benchmark_results.json --benchmark_out_format=json --benchmark_repetitions=1" pose_array_size:=1000 benchmarked_topic_name:="/dummy_benchmark_topic_name" benchmarked_topic_hz:=10 max_received_topic_number:=100
```

Let's explain some parameters used in benchmark.

| args | explanation |
| ---- | ----------- |
| `benchmark_command_args` | you can utilize this parameter to use custom benchmark arguments in your benchmark |
| `pose_array_size` | In this benchmark, [geometry_msgs/msg/PoseArray](https://docs.ros.org/en/rolling/p/geometry_msgs/interfaces/msg/PoseArray.html) is used. This parameter presents how many poses will be published from [basic_topic_publisher](../../src/scenarios/basic_topic_sub_pub/basic_topic_publisher.cpp). You can set message size sent by [basic_topic_publisher](../../src/scenarios/basic_topic_sub_pub/basic_topic_publisher.cpp) using this parameter. |
| `benchmarked_topic_hz` | This parameter presents in how many rate the message will be published from [basic_topic_publisher](../../src/scenarios/basic_topic_sub_pub/basic_topic_publisher.cpp). |
| `max_received_message_number` | This parameter sets how many message to handled by [scenario_basic_subscription benchmarking module](../../src/scenarios/basic_topic_sub_pub/scenario_basic_subscription.cpp). The more message number is handled, the more reliable benchmark results is achieved in `scenario_basic_subscription` scenario. |

## How to benchmark the basic topic subscription publishing works

The main idea here is to firstly save the message timestamp when published from [basic_topic_publisher](../../src/scenarios/basic_topic_sub_pub/basic_topic_publisher.cpp) and then handle this message in [scenario_basic_subscription benchmarking module](../../src/scenarios/basic_topic_sub_pub/scenario_basic_subscription.cpp) by which this message is received. This benchmark module uses [geometry_msgs/msg/PoseArray](https://docs.ros.org/en/rolling/p/geometry_msgs/interfaces/msg/PoseArray.html) which has arrangeable size and timestamp sections in message body. When message is received by [scenario_basic_subscription benchmarking module](../../src/scenarios/basic_topic_sub_pub/scenario_basic_subscription.cpp), the difference between the time message is received and the time message is sent is added to total elapsed time. In the end, this total elapsed time is saved to benchmark results.
