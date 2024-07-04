## How To Install


Firstly, setup the dependencies of moveit_middleware_benchmark repositories. It's suggested to test with rolling version of ROS.
```sh
# install colcon extensions
source /opt/ros/rolling/setup.bash
sudo apt install python3-colcon-common-extensions
sudo apt install python3-colcon-mixin
colcon mixin add default https://raw.githubusercontent.com/colcon/colcon-mixin-repository/master/index.yaml
colcon mixin update default

# create workspace
mkdir ws/src -p
cd ws/src

# clone this repository
git clone git@github.com:CihatAltiparmak/moveit_middleware_benchmark.git
vsc import moveit_middleware_benchmark/moveit_middleware_benchmark.repos --recursive

# build the workspace
cd ws
sudo apt update && rosdep install -r --from-paths . --ignore-src --rosdistro $ROS_DISTRO -y
colcon build --mixin release
```
