import os
from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from ament_index_python.packages import get_package_share_directory
from launch.actions import (
    DeclareLaunchArgument,
    ExecuteProcess,
    Shutdown,
    OpaqueFunction,
)
from launch_ros.substitutions import FindPackageShare
from launch_ros.actions import Node


def launch_setup(context, *args, **kwargs):
    benchmark_command_args = context.perform_substitution(
        LaunchConfiguration("benchmark_command_args")
    ).split()

    benchmarked_topic_hz = int(
        context.perform_substitution(LaunchConfiguration("benchmarked_topic_hz"))
    )

    benchmarked_topic_name = context.perform_substitution(
        LaunchConfiguration("benchmarked_topic_name")
    )

    max_receiving_topic_number = int(
        context.perform_substitution(LaunchConfiguration("max_receiving_topic_number"))
    )

    topic_publisher = ExecuteProcess(
        cmd=[
            [
                f"ros2 topic pub -r {benchmarked_topic_hz} {benchmarked_topic_name} std_msgs/msg/String 'data : 1'",
            ]
        ],
        shell=True,
    )

    benchmark_main_node = Node(
        name="benchmark_main",
        package="moveit_middleware_benchmark",
        executable="scenario_basic_subscription_benchmark_main",
        output="both",
        arguments=benchmark_command_args,
        parameters=[
            {"max_receiving_topic_number": max_receiving_topic_number},
            {"benchmarked_topic_name": benchmarked_topic_name},
            {"benchmarked_topic_hz": benchmarked_topic_hz},
        ],
        on_exit=Shutdown(),
    )

    return [topic_publisher, benchmark_main_node]


def generate_launch_description():
    declared_arguments = []

    benchmark_command_args = DeclareLaunchArgument(
        "benchmark_command_args",
        default_value="--benchmark_out=middleware_benchmark_results.json --benchmark_out_format=json --benchmark_repetitions=6",
        description="Google Benchmark Tool Arguments",
    )
    declared_arguments.append(benchmark_command_args)

    benchmarked_topic_hz_arg = DeclareLaunchArgument(
        "benchmarked_topic_hz", default_value="10000"
    )
    declared_arguments.append(benchmarked_topic_hz_arg)

    benchmarked_topic_name_arg = DeclareLaunchArgument(
        "benchmarked_topic_name", default_value="/benchmarked_topic1"
    )
    declared_arguments.append(benchmarked_topic_name_arg)

    max_receiving_topic_number_arg = DeclareLaunchArgument(
        "max_receiving_topic_number", default_value="1000000"
    )
    declared_arguments.append(max_receiving_topic_number_arg)

    return LaunchDescription(
        declared_arguments + [OpaqueFunction(function=launch_setup)]
    )
