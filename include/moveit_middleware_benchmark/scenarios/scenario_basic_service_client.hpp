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
   against basic service client works
 */

#pragma once

#include <rclcpp/rclcpp.hpp>
#include <benchmark/benchmark.h>
#include <memory>
#include <chrono>
#include <std_msgs/msg/string.hpp>
#include <example_interfaces/srv/add_two_ints.hpp>

#include <ament_index_cpp/get_package_share_directory.hpp>

namespace moveit
{
namespace middleware_benchmark
{

class ScenarioBasicServiceClient
{
public:
  /** \brief Constructor
   *  \param [in] node The ros node for sending request and wait response from server
   */
  ScenarioBasicServiceClient(rclcpp::Node::SharedPtr node);

  /** \brief the method to send the requests \e sending_request_number times
   * \param [in] sending_request_number the number for how many request is sent by client
   */
  void runTestCase(const int& sending_request_number);

private:
  rclcpp::Node::SharedPtr node_;
  rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedPtr client_;
};

class ScenarioBasicServiceClientFixture : public benchmark::Fixture
{
public:
  ScenarioBasicServiceClientFixture();

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
  /* The number to send request to server */
  int sending_request_number_;
};

}  // namespace middleware_benchmark
}  // namespace moveit
