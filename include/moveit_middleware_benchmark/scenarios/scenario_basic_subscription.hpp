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

#include <std_msgs/msg/string.hpp>
#include <dynmsg/msg_parser.hpp>
#include <dynmsg/typesupport.hpp>
#include <dynmsg/yaml_utils.hpp>

#include <ament_index_cpp/get_package_share_directory.hpp>

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

class ScenarioBasicSubPub
{
public:
  ScenarioBasicSubPub(rclcpp::Node::SharedPtr node);

  void runTestCase(const int& publishing_topic_number);
  void subCallback(std_msgs::msg::String::SharedPtr msg);

private:
  rclcpp::Node::SharedPtr node_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
  volatile int received_topic_number_;
};

class ScenarioBasicSubPubFixture : public benchmark::Fixture
{
public:
  ScenarioBasicSubPubFixture();

  void SetUp(::benchmark::State& /*state*/);

  void TearDown(::benchmark::State& /*state*/);

protected:
  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<rclcpp::executors::SingleThreadedExecutor> executor_;
  std::thread node_thread_;
  int max_receiving_topic_number_;
};

}  // namespace middleware_benchmark
}  // namespace moveit
