# moveit_middleware_benchmark

Under Construction

## How To Install


```sh
source /opt/ros/rolling/setup.bash
sudo apt install python3-colcon-common-extensions
sudo apt install python3-colcon-mixin
colcon mixin add default https://raw.githubusercontent.com/colcon/colcon-mixin-repository/master/index.yaml
colcon mixin update default
mkdir ws/src -p
cd ws/src
git clone git@github.com:CihatAltiparmak/moveit_middleware_benchmark.git -b fix/refactor_codebase
vsc import moveit_middleware_benchmark/moveit_middleware_benchmark.repos --recursive
cd ws
sudo apt update && rosdep install -r --from-paths . --ignore-src --rosdistro $ROS_DISTRO -y
colcon build --mixin release
```

## How To Run

```sh
source /opt/ros/rolling/setup.bash
source install/setup.bash
export RMW_IMPLEMENTATION=rmw_fastrtps_cpp # select your rmw_implementation to benchmark
ros2 launch moveit_middleware_benchmark scenario_perception_pipeline_benchmark.launch.py

```

It will be created the json file for benchmarking results after finishing benchmark code execution.
