## How To Run the MoveIt Task Constructor Benchmark

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

Select your rmw_implementation as `rmw_zenoh_cpp` and run the moveit task constructor benchmark launch file in the another terminal.
```sh
# go to your workspace
cd ws
source /opt/ros/rolling/setup.bash
source install/setup.bash
export RMW_IMPLEMENTATION=rmw_zenoh_cpp # select your rmw_implementation to benchmark
ros2 launch moveit_middleware_benchmark scenario_moveit_task_constructor_benchmark.launch.py
```

It will be defaultly benchmarked with 20 repetitions. It will be created the json file named `middleware_benchmark_results.json` for benchmarking results after finishing benchmark code execution. You can see the benchmark results in more detail inside this json file.

If you want to customize your benchmark arguments or select different test case, you can use below command.

```shell
ros2 launch moveit_middleware_benchmark scenario_moveit_task_constructor_benchmark.launch.py benchmark_command_args:="--benchmark_out=middleware_benchmark_results.json --benchmark_out_format=json --benchmark_repetitions=1"
```

## How to benchmark the MoveIt Task Constructor

The main idea here is to setup demo scene for operating task, to do planning task followed by executing task. After all of that, it's destroyed demo scene to conduct the benchmark of same task more than once. Thus, The effect of middleware on pick-place task demo can be measured the elapsed time reliably.

[moveit_task_constructor_benchmark.webm](https://gist.github.com/user-attachments/assets/c4848652-9522-4ccb-988d-3a80d1e5d23f)

## How to create test cases

You can apply some settings on pick-place task demo using [this parameter file in config directory](../../config/pick_place_demo_configs.yaml). In this benchmark scenario, it's enough to change `--benchmark-repetitions` through `benchmark_command_args` argument.
