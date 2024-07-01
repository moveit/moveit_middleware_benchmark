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
ros2 launch moveit_middleware_benchmark moveit_middleware_benchmark_demo.launch.py
```

It will be created the json file named `middleware_benchmark_results.json` for benchmarking results after finishing benchmark code execution. You can see the benchmark results in more detail inside this json file.

## How to benchmark the perception pipeline

The main idea here is to send some goal poses in `nav_msgs/msg/Path` format to move_group_server via move_group_interface and to measure the elapsed time.

## How to create test cases

You can add your test cases to `scenario_perception_pipeline_test_cases.yaml` file. For benchmarking of the scenario of perception pipeline, your test case must be presented in a type of nav_msgs/msg/Path. `poses` section presents `poses` field of `nav_msgs/msg/Path` type. There is a following example test case file. 

```yaml
test_cases:
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