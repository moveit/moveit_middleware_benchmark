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
   against topic subscription and publishing
 */

#include "moveit_middleware_benchmark/scenarios/scenario_basic_subscription.hpp"

namespace moveit
{
namespace middleware_benchmark
{

ScenarioBasicSubPub::ScenarioBasicSubPub(rclcpp::Node::SharedPtr node) : node_(node)
{
  received_topic_number_ = 0;
  node_->get_parameter("benchmarked_topic_name", benchmarked_topic_name_);
  node_->get_parameter("benchmarked_topic_hz", benchmarked_topic_hz_);
}

void ScenarioBasicSubPub::runTestCase(const int& max_received_topic_number)
{
  RCLCPP_INFO(node_->get_logger(), "Subscribing to topic : %s with hz %d", benchmarked_topic_name_.c_str(),
              benchmarked_topic_hz_);

  sub_ = node_->create_subscription<std_msgs::msg::String>(
      benchmarked_topic_name_, 10, std::bind(&ScenarioBasicSubPub::subCallback, this, std::placeholders::_1));

  RCLCPP_INFO(node_->get_logger(),
              "Successfully subscribed to topic %s with hz %d! When received msg number is bigger than %d, benchmark "
              "will be finished!",
              benchmarked_topic_name_.c_str(), benchmarked_topic_hz_, max_received_topic_number);

  while (received_topic_number_ < max_received_topic_number) {}

  RCLCPP_INFO(node_->get_logger(), "Benchmarked test case is finished!");
}

void ScenarioBasicSubPub::subCallback(std_msgs::msg::String::SharedPtr msg)
{
  received_topic_number_++;
}

ScenarioBasicSubPubFixture::ScenarioBasicSubPubFixture()
{
}

void ScenarioBasicSubPubFixture::SetUp(::benchmark::State& /*state*/)
{
  if (node_.use_count() == 0)
  {
    node_ = std::make_shared<rclcpp::Node>("test_scenario_basic_sub_pub",
                                           rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

    node_->get_parameter("max_receiving_topic_number", max_receiving_topic_number_);

    executor_ = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
    executor_->add_node(node_);
    node_thread_ = std::thread([this]() { executor_->spin(); });
  }
}

void ScenarioBasicSubPubFixture::TearDown(::benchmark::State& /*state*/)
{
  // Reset ros2 node shared pointers and their service, subscriber,
  // and publisher shared ptrs etc. before rclcpp::shutdown is not run.
  executor_->cancel();
  if (node_thread_.joinable())
    node_thread_.join();

  node_.reset();
}

BENCHMARK_DEFINE_F(ScenarioBasicSubPubFixture, test_scenario_basic_sub_pub)(benchmark::State& st)
{
  for (auto _ : st)
  {
    auto sc = ScenarioBasicSubPub(node_);
    sc.runTestCase(max_receiving_topic_number_);
  }
}

BENCHMARK_REGISTER_F(ScenarioBasicSubPubFixture, test_scenario_basic_sub_pub);

}  // namespace middleware_benchmark
}  // namespace moveit
