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

#include "moveit_middleware_benchmark/scenarios/basic_topic_sub_pub/scenario_basic_subscription.hpp"

namespace moveit
{
namespace middleware_benchmark
{

ScenarioBasicSubPub::ScenarioBasicSubPub(rclcpp::Node::SharedPtr node) : node_(node)
{
  is_test_case_finished_ = false;
  received_message_number_ = 0;
  node_->get_parameter("benchmarked_topic_name", benchmarked_topic_name_);
  node_->get_parameter("benchmarked_topic_hz", benchmarked_topic_hz_);
  node_->get_parameter("max_received_message_number", max_received_message_number_);
}

void ScenarioBasicSubPub::runTestCase(benchmark::State& benchmark_state)
{
  is_test_case_finished_ = false;

  RCLCPP_INFO(node_->get_logger(), "Subscribing to topic : %s with hz %d", benchmarked_topic_name_.c_str(),
              benchmarked_topic_hz_);

  sub_ = node_->create_subscription<geometry_msgs::msg::PoseArray>(
      benchmarked_topic_name_, 10, std::bind(&ScenarioBasicSubPub::subCallback, this, std::placeholders::_1));

  RCLCPP_INFO(node_->get_logger(),
              "Successfully subscribed to topic %s with hz %d! When received msg number is bigger than %ld, benchmark "
              "will be finished!",
              benchmarked_topic_name_.c_str(), benchmarked_topic_hz_, max_received_message_number_);

  std::unique_lock lk(is_test_case_finished_mutex_);
  test_case_cv_.wait(lk, [this]() { return is_test_case_finished_; });

  benchmark_state.SetIterationTime(elapsed_time_.count());
  RCLCPP_INFO(node_->get_logger(), "Benchmarked test case is finished!");
}

void ScenarioBasicSubPub::subCallback(geometry_msgs::msg::PoseArray::SharedPtr pose_array_msg)
{
  std::unique_lock lk(is_test_case_finished_mutex_);
  received_message_number_++;

  if (received_message_number_ > max_received_message_number_)
  {
    is_test_case_finished_ = true;
    lk.unlock();
    test_case_cv_.notify_one();
  }
  else
  {
    auto msg_latency_time = (node_->now() - pose_array_msg->header.stamp).to_chrono<std::chrono::duration<double>>();
    elapsed_time_ += msg_latency_time;
  }
}

ScenarioBasicSubPubFixture::ScenarioBasicSubPubFixture()
{
}

void ScenarioBasicSubPubFixture::SetUp(benchmark::State& /*state*/)
{
  if (node_.use_count() == 0)
  {
    node_ = std::make_shared<rclcpp::Node>("test_scenario_basic_sub_pub",
                                           rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

    executor_ = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
    executor_->add_node(node_);
    node_thread_ = std::thread([this]() { executor_->spin(); });
  }
}

void ScenarioBasicSubPubFixture::TearDown(benchmark::State& /*state*/)
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
    sc.runTestCase(st);
  }
}

BENCHMARK_REGISTER_F(ScenarioBasicSubPubFixture, test_scenario_basic_sub_pub)
    ->UseManualTime()
    ->Unit(benchmark::kNanosecond);

}  // namespace middleware_benchmark
}  // namespace moveit
