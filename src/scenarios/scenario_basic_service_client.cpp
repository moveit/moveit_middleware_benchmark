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

#include "moveit_middleware_benchmark/scenarios/scenario_basic_service_client.hpp"

namespace moveit
{
namespace middleware_benchmark
{

using namespace std::chrono_literals;

ScenarioBasicServiceClient::ScenarioBasicServiceClient(rclcpp::Node::SharedPtr node) : node_(node)
{
  client_ = node_->create_client<example_interfaces::srv::AddTwoInts>("add_two_ints");

  if (!client_->wait_for_service(5s))
  {
    RCLCPP_FATAL(node_->get_logger(), "Server is not available !");
  }

  // TODO @CihatAltiparmak : add this time stopings to
  //              perception pipeline benchmark as well
  // It should be waited even if server is okay due
  // to the fact that server sometimes doesn't response
  // request we sent in case that client sent request once server is ready
  std::this_thread::sleep_for(1s);
}

void ScenarioBasicServiceClient::runTestCase(const int& sending_request_number)
{
  for (int i = 0; i < sending_request_number; i++)
  {
    auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
    request->a = 5;
    request->b = 4;

    auto result = client_->async_send_request(request);

    if (rclcpp::spin_until_future_complete(node_, result) == rclcpp::FutureReturnCode::SUCCESS)
    {
      RCLCPP_DEBUG(node_->get_logger(), "Response %d is successfully returned by server!", i);
    }
    else
    {
      RCLCPP_ERROR(node_->get_logger(), "Request %d failed!", i);
    }
  }
}

ScenarioBasicServiceClientFixture::ScenarioBasicServiceClientFixture()
{
}

void ScenarioBasicServiceClientFixture::SetUp(::benchmark::State& /*state*/)
{
  if (node_.use_count() == 0)
  {
    node_ = std::make_shared<rclcpp::Node>("test_scenario_basic_service_client",
                                           rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

    node_->get_parameter("sending_request_number", sending_request_number_);
  }
}

void ScenarioBasicServiceClientFixture::TearDown(::benchmark::State& /*state*/)
{
  node_.reset();
}

BENCHMARK_DEFINE_F(ScenarioBasicServiceClientFixture, test_scenario_basic_service_client)(benchmark::State& st)
{
  for (auto _ : st)
  {
    // TODO @CihatAltiparmak : add this time stopings to
    //              perception pipeline benchmark as well
    st.PauseTiming();
    auto sc = ScenarioBasicServiceClient(node_);
    st.ResumeTiming();

    sc.runTestCase(sending_request_number_);
  }
}

BENCHMARK_REGISTER_F(ScenarioBasicServiceClientFixture, test_scenario_basic_service_client);

}  // namespace middleware_benchmark
}  // namespace moveit
