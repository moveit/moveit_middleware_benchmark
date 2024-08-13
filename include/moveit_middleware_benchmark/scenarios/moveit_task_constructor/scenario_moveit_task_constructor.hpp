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
   against moveit_task_constructor
 */

#pragma once

#include <rclcpp/rclcpp.hpp>
#include <benchmark/benchmark.h>
#include <memory>

#include <ament_index_cpp/get_package_share_directory.hpp>

#include <moveit/moveit_cpp/moveit_cpp.h>
#include <moveit/moveit_cpp/planning_component.h>
#include <moveit/move_group_interface/move_group_interface.h>

#include "moveit_middleware_benchmark/scenarios/moveit_task_constructor/pick_place_task.hpp"

namespace
{
}  // namespace

namespace moveit
{
namespace middleware_benchmark
{

class ScenarioMoveItTaskConstructor
{
public:
  ScenarioMoveItTaskConstructor(const rclcpp::Node::SharedPtr & node);
  ~ScenarioMoveItTaskConstructor();

  void runTestCase();

private:
  std::shared_ptr<pick_place_task_demo::ParamListener> pick_place_task_param_listener_;
  pick_place_task_demo::Params pick_place_task_demo_parameters_;
  std::shared_ptr<moveit_task_constructor_demo::PickPlaceTask> pick_place_task_;
  rclcpp::Node::SharedPtr node_;
};

class ScenarioMoveItTaskConstructorFixture : public benchmark::Fixture
{
public:
  ScenarioMoveItTaskConstructorFixture();

  void SetUp(::benchmark::State& /*state*/);

  void TearDown(::benchmark::State& /*state*/);

protected:
  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<rclcpp::executors::SingleThreadedExecutor> node_executor_;
  std::unique_ptr<std::thread> spinning_thread_;
};

}  // namespace middleware_benchmark
}  // namespace moveit