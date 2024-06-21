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
   against perception pipeline of
 */

#pragma once

#include <rclcpp/rclcpp.hpp>
#include <benchmark/benchmark.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <memory>

#include <moveit/moveit_cpp/moveit_cpp.h>
#include <moveit/moveit_cpp/planning_component.h>

#include <geometry_msgs/msg/pose.hpp>
#include <nav_msgs/msg/path.hpp>
#include <dynmsg/msg_parser.hpp>
#include <dynmsg/typesupport.hpp>
#include <dynmsg/yaml_utils.hpp>

#include <ament_index_cpp/get_package_share_directory.hpp>

namespace moveit
{
namespace middleware_benchmark
{

using moveit::planning_interface::MoveGroupInterface;

const std::string PLANNING_GROUP = "panda_arm";

class ScenarioPerceptionPipeline
{
public:
  ScenarioPerceptionPipeline(std::shared_ptr<MoveGroupInterface>);
  std::tuple<int, int> runTestCase(const nav_msgs::msg::Path& pose_list);
  bool sendTargetPose(const geometry_msgs::msg::Pose& target_pose);

private:
  std::shared_ptr<MoveGroupInterface> move_group_interface_ptr_;
};

class ScenarioPerceptionPipelineTestCaseCreator
{
private:
  static inline std::vector<nav_msgs::msg::Path> test_cases_ = {};

public:
  static void createTestCases();

  static nav_msgs::msg::Path selectTestCases(size_t test_case_index);

  static void readTestCasesFromFile(const std::string& yaml_file_name);

  static nav_msgs::msg::Path getTestCaseFromYamlString(const std::string& yaml_string);
};

class ScenarioPerceptionPipelineFixture : public benchmark::Fixture
{
protected:
  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<MoveGroupInterface> move_group_interface_ptr_;
  size_t selected_test_case_index_;

public:
  ScenarioPerceptionPipelineFixture();
  void SetUp(::benchmark::State& /*state*/);
  void TearDown(::benchmark::State& /*state*/);
};

}  // namespace middleware_benchmark
}  // namespace moveit
