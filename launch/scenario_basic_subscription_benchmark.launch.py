import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, Shutdown
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch.conditions import IfCondition
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from ament_index_python.packages import get_package_share_directory
from launch.actions import (
    DeclareLaunchArgument,
    ExecuteProcess,
    LogInfo,
    RegisterEventHandler,
)
from launch.event_handlers import OnProcessExit


def generate_launch_description():
    benchmarked_topic_hz = LaunchConfiguration("benchmarked_topic_hz")
    benchmarked_topic_hz_arg = DeclareLaunchArgument(
        "benchmarked_topic_hz", default_value="1000"
    )

    benchmarked_topic_name = LaunchConfiguration("bencmarked_topic_name")
    benchmarked_topic_name_arg = DeclareLaunchArgument(
        "bencmarked_topic_name", default_value="/benchmarked_topic1"
    )

    topic_publisher = ExecuteProcess(
        cmd=[
            [
                f"ros2 topic pub -r ",
                benchmarked_topic_hz,
                " ",
                benchmarked_topic_name,
                " std_msgs/msg/String ",
                '"data : 1"',
            ]
        ],
        shell=True,
    )

    benchmark_main_node = Node(
        name="benchmark_main",
        package="moveit_middleware_benchmark",
        executable="scenario_basic_subscription_benchmark_main",
        output="both",
        arguments=[
            "--benchmark_out=middleware_benchmark_results.json",
            "--benchmark_out_format=json",
        ],
        parameters=[
            {"max_receiving_topic_number": 100000},
        ],
        on_exit=Shutdown(),
    )

    return LaunchDescription(
        [
            benchmarked_topic_hz_arg,
            benchmarked_topic_name_arg,
            topic_publisher,
            benchmark_main_node,
        ]
    )
