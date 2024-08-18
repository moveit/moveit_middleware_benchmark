import os
from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from ament_index_python.packages import get_package_share_directory, get_package_prefix
from launch.actions import (
    DeclareLaunchArgument,
    ExecuteProcess,
    Shutdown,
    OpaqueFunction,
)
from launch_ros.substitutions import FindPackageShare
from launch_ros.actions import Node
import subprocess

"""
Zenoh router needs to use /dev/tty as stdin. We cannot set stdin in ExecuteProcess.
The functionality of launch tool doesn't allow to set stdin, Even if we use
shell=True parameter and set stdin using ros2 run rmw_zenoh_cpp rmw_zenoh < /dev/tty,
launch closes process unsoundly, it somehow trigger neither the SIGTERM nor SIGINT.
I could solve this using separate subprocess mechanism, which allows us to modify stdin of process directly.
See my comment in this issue. ( https://github.com/ros2/rmw_zenoh/issues/206#issuecomment-2257292941 )

TODO: (CihatAltiparmak) Remove this class when rmw_zenoh becomes runnable without starting router
separately or when tty option is removed from router executable
"""


class ZenohRouterStarter:
    def __init__(self):
        self.tty_file_descriptor = None
        self.zenoh_router_sub_process = None

    def start_router(self):
        try:
            rmw_zenoh_cpp_router_executable_path = os.path.join(
                get_package_prefix("rmw_zenoh_cpp"), "lib/rmw_zenoh_cpp/rmw_zenohd"
            )

            self.tty_file_descriptor = open("/dev/tty")
            self.zenoh_router_sub_process = subprocess.Popen(
                [rmw_zenoh_cpp_router_executable_path],
                stdin=self.tty_file_descriptor,
            )
        except Exception as err:
            print(f"Unexpected {err=}, {type(err)=}")
            raise

    def terminate_router(self):
        self.zenoh_router_sub_process.terminate()
        self.zenoh_router_sub_process.wait()

    def __del__(self):
        if self.tty_file_descriptor is not None:
            self.tty_file_descriptor.close()


# for rmw_zenoh_router
zenoh_router_starter = ZenohRouterStarter()
if (
    "RMW_IMPLEMENTATION" in os.environ.keys()
    and os.environ.get("RMW_IMPLEMENTATION") == "rmw_zenoh_cpp"
):
    zenoh_router_starter.start_router()


def shutdown_launch():
    if (
        "RMW_IMPLEMENTATION" in os.environ.keys()
        and os.environ.get("RMW_IMPLEMENTATION") == "rmw_zenoh_cpp"
    ):
        zenoh_router_starter.terminate_router()
    Shutdown()()


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
        on_exit=lambda *args: shutdown_launch(),
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
