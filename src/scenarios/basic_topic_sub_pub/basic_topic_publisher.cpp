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
   Description: Basic pose_array_msg topic publisher to measure the middleware
   effects in basic topic subscription-publishing scenario
 */

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose_array.hpp>

int main(int argc, char** argv)
{
  size_t pose_array_size = 10;
  double benchmarked_topic_hz = 10;
  std::string benchmarked_topic_name = "/benchmarked_topic1";

  rclcpp::init(argc, argv);

  auto node = rclcpp::Node::make_shared("basic_topic_publisher");

  node->get_parameter("benchmarked_topic_hz", benchmarked_topic_hz);
  node->get_parameter("benchmarked_topic_name", benchmarked_topic_name);
  node->get_parameter("pose_array_size", pose_array_size);

  auto topic_pub = node->create_publisher<geometry_msgs::msg::PoseArray>(benchmarked_topic_name, 10);

  geometry_msgs::msg::PoseArray sample_pose_array_msg;
  sample_pose_array_msg.poses.resize(pose_array_size);

  rclcpp::Rate sleep_rate(benchmarked_topic_hz);

  while (rclcpp::ok())
  {
    // save the publishing date of message
    sample_pose_array_msg.header.stamp = node->now();

    // publish the message
    topic_pub->publish(sample_pose_array_msg);

    sleep_rate.sleep();
  }

  rclcpp::shutdown();
}
