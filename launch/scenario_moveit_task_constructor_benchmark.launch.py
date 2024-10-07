import os
from launch import LaunchDescription
from launch.substitutions import (
    LaunchConfiguration,
    PathJoinSubstitution,
    EnvironmentVariable,
    EqualsSubstitution,
)
from launch.conditions import IfCondition
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
from launch.actions import (
    DeclareLaunchArgument,
    ExecuteProcess,
    Shutdown,
    LogInfo,
    RegisterEventHandler,
    OpaqueFunction,
)
from launch.event_handlers import OnProcessExit
from launch_ros.substitutions import FindPackageShare
from moveit_configs_utils import MoveItConfigsBuilder


def launch_setup(context, *args, **kwargs):
    benchmark_command_args = context.perform_substitution(
        LaunchConfiguration("benchmark_command_args")
    ).split()

    moveit_config = (
        MoveItConfigsBuilder("moveit_resources_panda")
        .robot_description(
            file_path="config/panda.urdf.xacro",
            mappings={
                "ros2_control_hardware_type": LaunchConfiguration(
                    "ros2_control_hardware_type"
                )
            },
        )
        .robot_description_semantic(file_path="config/panda.srdf")
        .robot_description_kinematics(file_path="config/kinematics.yaml")
        .planning_scene_monitor(
            publish_robot_description=True, publish_robot_description_semantic=True
        )
        .trajectory_execution(file_path="config/gripper_moveit_controllers.yaml")
        .planning_pipelines(pipelines=["ompl"])
        .to_moveit_configs()
    )

    # Load  ExecuteTaskSolutionCapability so we can execute found solutions in simulation
    move_group_capabilities = {
        "capabilities": "move_group/ExecuteTaskSolutionCapability"
    }

    # Start the actual move_group node/action server
    move_group_node = Node(
        package="moveit_ros_move_group",
        executable="move_group",
        output="screen",
        parameters=[moveit_config.to_dict(), move_group_capabilities],
        arguments=["--ros-args", "--log-level", "info"],
    )

    # Static TF
    static_tf_node = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="static_transform_publisher",
        output="log",
        arguments=["0.0", "0.0", "0.0", "0.0", "0.0", "0.0", "world", "panda_link0"],
    )

    # Publish TF
    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        name="robot_state_publisher",
        output="both",
        parameters=[moveit_config.robot_description],
    )

    # ros2_control using FakeSystem as hardware
    ros2_controllers_path = os.path.join(
        get_package_share_directory("moveit_resources_panda_moveit_config"),
        "config",
        "ros2_controllers.yaml",
    )
    ros2_control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[ros2_controllers_path],
        remappings=[
            ("/controller_manager/robot_description", "/robot_description"),
        ],
        output="screen",
    )

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "joint_state_broadcaster",
            "--controller-manager",
            "/controller_manager",
        ],
    )

    panda_arm_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["panda_arm_controller", "-c", "/controller_manager"],
    )

    panda_hand_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["panda_hand_controller", "-c", "/controller_manager"],
    )

    zenoh_router = Node(
        name="zenoh_router",
        package="rmw_zenoh_cpp",
        executable="rmw_zenohd",
        output="both",
        condition=IfCondition(
            EqualsSubstitution(
                EnvironmentVariable("RMW_IMPLEMENTATION", default_value=""),
                "rmw_zenoh_cpp",
            ),
        ),
    )

    benchmark_main_node = Node(
        name="benchmark_main",
        package="moveit_middleware_benchmark",
        executable="scenario_moveit_task_constructor_benchmark_main",
        output="both",
        arguments=benchmark_command_args,
        parameters=[
            os.path.join(
                get_package_share_directory("moveit_middleware_benchmark"),
                "config",
                "pick_place_demo_configs.yaml",
            ),
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.planning_pipelines,
            moveit_config.joint_limits,
            {"use_sim_time": True},
        ],
        on_exit=Shutdown(),
    )

    return [
        zenoh_router,
        move_group_node,
        static_tf_node,
        robot_state_publisher,
        ros2_control_node,
        joint_state_broadcaster_spawner,
        panda_arm_controller_spawner,
        panda_hand_controller_spawner,
        # for https://github.com/ros-controls/ros2_controllers/issues/981
        RegisterEventHandler(
            OnProcessExit(
                target_action=panda_arm_controller_spawner,
                on_exit=[
                    LogInfo(
                        msg="panda_arm_controller_spawner is finished. Now test_scenario_perception_pipeline will start"
                    ),
                    benchmark_main_node,
                ],
            )
        ),
    ]


def generate_launch_description():
    declared_arguments = []

    benchmark_command_args = DeclareLaunchArgument(
        "benchmark_command_args",
        default_value="--benchmark_out=middleware_benchmark_results.json --benchmark_out_format=json --benchmark_repetitions=20",
        description="Google Benchmark Tool Arguments",
    )
    declared_arguments.append(benchmark_command_args)

    ros2_control_hardware_type = DeclareLaunchArgument(
        "ros2_control_hardware_type",
        default_value="mock_components",
        description="ROS 2 control hardware interface type to use for the launch file -- possible values: [mock_components, isaac]",
    )
    declared_arguments.append(ros2_control_hardware_type)

    return LaunchDescription(
        declared_arguments + [OpaqueFunction(function=launch_setup)]
    )
