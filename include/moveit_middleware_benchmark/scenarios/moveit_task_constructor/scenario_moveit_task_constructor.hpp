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

#include "moveit_middleware_benchmark/scenarios/moveit_task_constructor/pick_place_task.hpp"

namespace moveit
{
namespace middleware_benchmark
{

class ScenarioMoveItTaskConstructor
{
public:
  /** \brief Firstly, prepares the demo scene for pick-place actions
   *  and then initializes the task which created by moveit_task_constructor,
   *  \param [in] node The ros node
   */
  ScenarioMoveItTaskConstructor(const rclcpp::Node::SharedPtr& node);

  /** \brief Firstly, destroys the created demo scene in order to reprepare
   *  the environment for pick-place actions
   */
  ~ScenarioMoveItTaskConstructor();

  /** \brief Starts to plan to in order to execute the task which
   *  created by moveit_task_constructor and then if task planning is successful,
   *  task plan is executed
   */
  void runTestCase();

private:
  /* the interface to get needed parameters for pick-place demo*/
  std::shared_ptr<pick_place_task_demo::ParamListener> pick_place_task_param_listener_;

  /* the gotten parameters in order to be used for pick-place demo*/
  pick_place_task_demo::Params pick_place_task_demo_parameters_;

  /* The pick-place demo instance to process the executing and planning operations on robot*/
  std::shared_ptr<moveit_task_constructor_demo::PickPlaceTask> pick_place_task_;

  rclcpp::Node::SharedPtr node_;
};

class ScenarioMoveItTaskConstructorFixture : public benchmark::Fixture
{
public:
  ScenarioMoveItTaskConstructorFixture();

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
  std::shared_ptr<rclcpp::executors::SingleThreadedExecutor> node_executor_;
  std::unique_ptr<std::thread> spinning_thread_;
};

}  // namespace middleware_benchmark
}  // namespace moveit
