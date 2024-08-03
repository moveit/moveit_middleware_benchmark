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

    sending_request_number = int(
        context.perform_substitution(LaunchConfiguration("sending_request_number"))
    )

    add_two_ints_server_node = Node(
        name="add_two_ints_server",
        package="demo_nodes_cpp",
        executable="add_two_ints_server",
    )

    benchmark_main_node = Node(
        name="benchmark_main",
        package="moveit_middleware_benchmark",
        executable="scenario_basic_service_client_benchmark_main",
        output="both",
        arguments=benchmark_command_args,
        parameters=[
            {"sending_request_number": sending_request_number},
        ],
        on_exit=Shutdown(),
    )

    return [add_two_ints_server_node, benchmark_main_node]


def generate_launch_description():
    declared_arguments = []

    benchmark_command_args = DeclareLaunchArgument(
        "benchmark_command_args",
        default_value="--benchmark_out=middleware_benchmark_results.json --benchmark_out_format=json --benchmark_repetitions=6",
        description="Google Benchmark Tool Arguments",
    )
    declared_arguments.append(benchmark_command_args)

    sending_request_number_arg = DeclareLaunchArgument(
        "sending_request_number", default_value="10000"
    )
    declared_arguments.append(sending_request_number_arg)

    return LaunchDescription(
        declared_arguments + [OpaqueFunction(function=launch_setup)]
    )
