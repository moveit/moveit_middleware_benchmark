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

#include "moveit_middleware_benchmark/scenarios/scenario_perception_pipeline.hpp"

namespace moveit
{
namespace middleware_benchmark
{

ScenarioPerceptionPipeline::ScenarioPerceptionPipeline(std::shared_ptr<MoveGroupInterface> move_group_interface_ptr)
  : move_group_interface_ptr_(move_group_interface_ptr)
{
}

std::tuple<int, int> ScenarioPerceptionPipeline::runTestCase(const nav_msgs::msg::Path& test_case)
{
  int success_number = 0;
  int failure_number = 0;
  for (auto& pose_stamped : test_case.poses)
  {
    bool is_successful = sendTargetPose(pose_stamped.pose);
    if (is_successful)
      success_number++;
    else
      failure_number++;
  }

  return { success_number, failure_number };
}

bool ScenarioPerceptionPipeline::sendTargetPose(const geometry_msgs::msg::Pose& target_pose)
{
  move_group_interface_ptr_->setPoseTarget(target_pose);

  moveit::planning_interface::MoveGroupInterface::Plan plan_msg;
  if (move_group_interface_ptr_->plan(plan_msg) == moveit::core::MoveItErrorCode::SUCCESS)
  {
    move_group_interface_ptr_->execute(plan_msg);
    return true;
  }
  else
  {
    return false;
  }
}

ScenarioPerceptionPipelineFixture::ScenarioPerceptionPipelineFixture()
{
  createTestCases();
}

void ScenarioPerceptionPipelineFixture::SetUp(::benchmark::State& /*state*/)
{
  if (node_.use_count() == 0)
  {
    node_ = std::make_shared<rclcpp::Node>("test_scenario_perception_pipeline_node",
                                           rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

    node_->get_parameter("selected_test_case_index", selected_test_case_index_);
  }

  if (move_group_interface_ptr_.use_count() == 0)
  {
    move_group_interface_ptr_ = std::make_shared<MoveGroupInterface>(node_, PLANNING_GROUP);
  }
}

void ScenarioPerceptionPipelineFixture::TearDown(::benchmark::State& /*state*/)
{
}

void ScenarioPerceptionPipelineFixture::createTestCases()
{
  readTestCasesFromFile(TEST_CASES_YAML_FILE);
}

const nav_msgs::msg::Path ScenarioPerceptionPipelineFixture::selectTestCase(size_t test_case_index)
{
  return test_cases_.at(test_case_index);
}

void ScenarioPerceptionPipelineFixture::readTestCasesFromFile(const std::string& yaml_file_name)
{
  YAML::Node config = YAML::LoadFile(yaml_file_name.c_str());
  for (YAML::const_iterator it = config["test_cases"].begin(); it != config["test_cases"].end(); ++it)
  {
    const std::string yaml_string = dynmsg::yaml_to_string(*it);
    nav_msgs::msg::Path test_case = getTestCaseFromYamlString(yaml_string);

    test_cases_.push_back(test_case);
  }
}

nav_msgs::msg::Path ScenarioPerceptionPipelineFixture::getTestCaseFromYamlString(const std::string& yaml_string)
{
  nav_msgs::msg::Path path_msg;
  void* ros_message = reinterpret_cast<void*>(&path_msg);

  dynmsg::cpp::yaml_and_typeinfo_to_rosmsg(dynmsg::cpp::get_type_info({ "nav_msgs", "Path" }), yaml_string, ros_message);

  return path_msg;
}

BENCHMARK_DEFINE_F(ScenarioPerceptionPipelineFixture, test_scenario_perception_pipeline)(benchmark::State& st)
{
  auto selected_test_case = selectTestCase(selected_test_case_index_);
  for (auto _ : st)
  {
    auto sc = ScenarioPerceptionPipeline(move_group_interface_ptr_);
    auto [success_number, failure_number] = sc.runTestCase(selected_test_case);
    st.counters["success_number"] = success_number;
    st.counters["failure_number"] = failure_number;
  }
}

BENCHMARK_REGISTER_F(ScenarioPerceptionPipelineFixture, test_scenario_perception_pipeline);

}  // namespace middleware_benchmark
}  // namespace moveit
