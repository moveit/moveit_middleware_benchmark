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

#pragma once

#include <rclcpp/rclcpp.hpp>
#include <benchmark/benchmark.h>
#include <memory>
#include <mutex>
#include <geometry_msgs/msg/pose_array.hpp>

#include <ament_index_cpp/get_package_share_directory.hpp>

namespace moveit
{
namespace middleware_benchmark
{

class ScenarioBasicSubPub
{
public:
  /** \brief Constructor
   *  \param [in] node The ros node for subscribing the benchmarking topic
   * and getting the necessary ROS params
   */
  ScenarioBasicSubPub(rclcpp::Node::SharedPtr node);

  /** \brief the method to measure the message latency
   * by handling maximum \e max_received_message_number_ message
   * \param [in] benchmark_state the google benchmark instance
   * to save message latency to benchmark results
   */
  void runTestCase(benchmark::State& benchmark_state);

  /** \brief the method to subscribe ROS message and add
   * the message latencies to total elapsed time until
   * \e max_received_message_number_ message is received
   * \param [in] msg ROS message
   */
  void subCallback(geometry_msgs::msg::PoseArray::SharedPtr msg);

private:
  rclcpp::Node::SharedPtr node_;
  rclcpp::Subscription<geometry_msgs::msg::PoseArray>::SharedPtr sub_;
  size_t received_message_number_;

  /* max message number to be able to handle */
  size_t max_received_message_number_;

  /* topic name to subscribe determined topic */
  std::string benchmarked_topic_name_;

  /* topic publishing hz */
  int benchmarked_topic_hz_;

  std::condition_variable test_case_cv_;
  std::mutex is_test_case_finished_mutex_;
  bool is_test_case_finished_;

  /* total message latency */
  std::chrono::duration<double> elapsed_time_;
};

class ScenarioBasicSubPubFixture : public benchmark::Fixture
{
public:
  ScenarioBasicSubPubFixture();

  /** \brief This method runs once each benchmark starts
   *  \param [in] state
   */
  void SetUp(::benchmark::State& /*state*/);

  /** \brief This method runs as soon as each benchmark finishes
   *  \param [in] state
   */
  void TearDown(::benchmark::State& /*state*/);

protected:
  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<rclcpp::executors::SingleThreadedExecutor> executor_;
  std::thread node_thread_;
};

}  // namespace middleware_benchmark
}  // namespace moveit
