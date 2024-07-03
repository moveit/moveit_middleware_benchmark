## How To Run Perception Benchmark

Firstly, source your ros version. It's suggested to test with rolling version of ROS.

For instance, to test with rmw_zenoh, start to zenoh router using following command
```sh
# go to your workspace
cd ws
# Be sure that ros2 daemon is killed.
pkill -9 -f ros && ros2 daemon stop
# Then start zenoh router
ros2 run rmw_zenoh_cpp rmw_zenohd
```

Select your rmw_implementation as `rmw_zenoh_cpp` and run the perception benchmark launch file.
```sh
# go to your workspace
cd ws
source /opt/ros/rolling/setup.bash
source install/setup.bash
export RMW_IMPLEMENTATION=rmw_zenoh_cpp # select your rmw_implementation to benchmark
ros2 launch moveit_middleware_benchmark scenario_perception_pipeline_benchmark.launch.py
```

It will be created the json file named `middleware_benchmark_results.json` for benchmarking results after finishing benchmark code execution. You can see the benchmark results in more detail inside this json file.

## How to benchmark the perception pipeline

The main idea here is to send some goal poses in `nav_msgs/msg/Path` format to move_group_server via move_group_interface and to measure the elapsed time.

## How to create test cases

You can add your test cases to `scenario_perception_pipeline_test_cases.yaml` file. For benchmarking of the scenario of perception pipeline, your test case must be presented in a type of nav_msgs/msg/Path. `poses` section presents `poses` field of `nav_msgs/msg/Path` type. There is a following example test case file.

```yaml
# test cases stored in the nav_msgs/msg/Path
test_cases:
  # the first test case in type of nav_msgs/msg/Path,
  # This test case can be selected in scenario_perception_pipeline_benchmark_main node
  # by passing selected_test_case_index argument as 0
  - poses:
    # one of the pose in the `poses` field of nav_msgs/msg/Path
    - pose:
        position:
          x: 0.5
          y: 0.5
          z: 0.5
        orientation:
          w : 1.0
    - pose:
        position:
          x: 0.5
          y: -0.5
          z: 0.7
        orientation:
          w: 1.0
  # the second test case in type of nav_msgs/msg/Path
  # This test case can be selected in scenario_perception_pipeline_benchmark_main node
  # by passing selected_test_case_index argument as 0
  # When you select this test case, robot firstly tries to go the first pose.
  # After going to first pose, robot tries to go second pose. Then robot tries
  # to go the first pose again. Robot tries to go between two poses 10 times.
  # (In totally, robot has 20 goal poses)
  # To give more goal pose increases the measurement reliability of middleware effects.
  # Because sampling based planner selection affects the benchmark measurements negatively.
  - poses:
    - pose:
        position:
          x: 0.5
          y: 0.5
          z: 0.5
        orientation:
          w : 1.0
    - pose:
        position:
          x: 0.5
          y: -0.5
          z: 0.7
        orientation:
          w: 1.0
    - pose:
        position:
          x: 0.5
          y: 0.5
          z: 0.5
        orientation:
          w : 1.0
    - pose:
        position:
          x: 0.5
          y: -0.5
          z: 0.7
        orientation:
          w: 1.0
    - pose:
        position:
          x: 0.5
          y: 0.5
          z: 0.5
        orientation:
          w : 1.0
    - pose:
        position:
          x: 0.5
          y: -0.5
          z: 0.7
        orientation:
          w: 1.0
    - pose:
        position:
          x: 0.5
          y: 0.5
          z: 0.5
        orientation:
          w : 1.0
    - pose:
        position:
          x: 0.5
          y: -0.5
          z: 0.7
        orientation:
          w: 1.0
    - pose:
        position:
          x: 0.5
          y: 0.5
          z: 0.5
        orientation:
          w : 1.0
    - pose:
        position:
          x: 0.5
          y: -0.5
          z: 0.7
        orientation:
          w: 1.0
    - pose:
        position:
          x: 0.5
          y: 0.5
          z: 0.5
        orientation:
          w : 1.0
    - pose:
        position:
          x: 0.5
          y: -0.5
          z: 0.7
        orientation:
          w: 1.0
    - pose:
        position:
          x: 0.5
          y: 0.5
          z: 0.5
        orientation:
          w : 1.0
    - pose:
        position:
          x: 0.5
          y: -0.5
          z: 0.7
        orientation:
          w: 1.0
    - pose:
        position:
          x: 0.5
          y: 0.5
          z: 0.5
        orientation:
          w : 1.0
    - pose:
        position:
          x: 0.5
          y: -0.5
          z: 0.7
        orientation:
          w: 1.0
    - pose:
        position:
          x: 0.5
          y: 0.5
          z: 0.5
        orientation:
          w : 1.0
    - pose:
        position:
          x: 0.5
          y: -0.5
          z: 0.7
        orientation:
          w: 1.0
    - pose:
        position:
          x: 0.5
          y: 0.5
          z: 0.5
        orientation:
          w : 1.0
    - pose:
        position:
          x: 0.5
          y: -0.5
          z: 0.7
        orientation:
          w: 1.0

```
