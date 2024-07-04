/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2024, Cihat Kurtuluş Altıparmak
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of PickNik Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Cihat Kurtuluş Altıparmak
   Description: Benchmarking module to compare the effects of middlewares
   against perception pipeline
 */

#pragma once

#include <rclcpp/rclcpp.hpp>
#include <benchmark/benchmark.h>
#include <memory>

#include <geometry_msgs/msg/pose.hpp>
#include <nav_msgs/msg/path.hpp>
#include <dynmsg/msg_parser.hpp>
#include <dynmsg/typesupport.hpp>
#include <dynmsg/yaml_utils.hpp>

#include <ament_index_cpp/get_package_share_directory.hpp>

#include <moveit/moveit_cpp/moveit_cpp.h>
#include <moveit/moveit_cpp/planning_component.h>
#include <moveit/move_group_interface/move_group_interface.h>

namespace
{
const std::string PLANNING_GROUP = "panda_arm";
const std::string PACKAGE_SHARE_DIRECTORY = ament_index_cpp::get_package_share_directory("moveit_middleware_benchmark");
const std::string TEST_CASES_YAML_FILE =
    PACKAGE_SHARE_DIRECTORY + "/config/scenario_perception_pipeline_test_cases.yaml";
}  // namespace

namespace moveit
{
namespace middleware_benchmark
{

using moveit::planning_interface::MoveGroupInterface;

class ScenarioPerceptionPipeline
{
public:
  /** \brief Constructor for interface to send goal poses
   *  \param [in] move_group_interface_ptr The move_group_interface for sending goal poses
   */
  ScenarioPerceptionPipeline(std::shared_ptr<MoveGroupInterface> move_group_interface_ptr);

  /** \brief Given a \e pose_list, sends poses in pose_list to plan,
   *  if the pipeline in \e sendTargetPose returns true,
   *  then success_number is increased.
   *  If the pipeline in \e sendTargetPose returns false,
   *  failure_number is increased.
   *  \param [in] test_case The pose list to benchmark
   *  \return success_number and failure_number in tuple respectively
   */
  std::tuple<int, int> runTestCase(const nav_msgs::msg::Path& pose_list);

  /** \brief Given a \e target_pose, sends this pose in order to plan via move_group_interface,
   *  if the planning is successful,
   *  planning message is sent to execute trajectory. Then it is returned true.
   *  If the planning is failed, the execution is ignored and then it is returned false
   *  \param [in] target_pose The pose to plan
   *  \return if the planning successful, returns true, or false
   */
  bool sendTargetPose(const geometry_msgs::msg::Pose& target_pose);

private:
  /** move_group_interface_ptr to communicate with move_group_server */
  std::shared_ptr<MoveGroupInterface> move_group_interface_ptr_;
};

class ScenarioPerceptionPipelineFixture : public benchmark::Fixture
{
public:
  ScenarioPerceptionPipelineFixture();

  /** \brief This method runs once each benchmark starts
   *  \param [in] state
   */
  void SetUp(::benchmark::State& /*state*/);

  /** \brief This method runs as soon as each benchmark finishes
   *  \param [in] state
   */
  void TearDown(::benchmark::State& /*state*/);

  /** \brief Creates test_cases by filling \e test_cases_
   *  If the planning is failed, the execution is ignored and then it is returned false
   */
  void createTestCases();

  /** \brief Given a \e test_case_index, returns relevant test case.
   *  \param [in] test_case_index The index of test_case
   *  \return selected test case
   */
  const nav_msgs::msg::Path selectTestCase(size_t test_case_index);

  /** \brief Given a \e yaml_file_name, reads test cases from given yaml file
   *  and adds this read test cases to \e test_cases_
   *  \param [in] yaml_file_name the path of yaml file which includes test cases
   */
  void readTestCasesFromFile(const std::string& yaml_file_name);

  /** \brief Given a \e yaml_string, converts yaml_string to ros message
   *  and returns this ros message.
   *  \param [in] yaml_string the yaml string storing ros message fields
   *  \return nav_msgs::msg::Path which contains pose_list is returned
   */
  nav_msgs::msg::Path getTestCaseFromYamlString(const std::string& yaml_string);

protected:
  rclcpp::Node::SharedPtr node_;

  /* move_group_interface_ptr for communicating with move_group_server */
  std::shared_ptr<MoveGroupInterface> move_group_interface_ptr_;

  /* selected test case index for benchmarking */
  size_t selected_test_case_index_;

  /** the list of test cases to be tested */
  std::vector<nav_msgs::msg::Path> test_cases_ = {};
};

}  // namespace middleware_benchmark
}  // namespace moveit
