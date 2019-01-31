/*
 * Copyright 2018 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <string>
#include <gtest/gtest.h>

#include "sdf/Element.hh"
#include "sdf/Filesystem.hh"
#include "sdf/Joint.hh"
#include "sdf/JointAxis.hh"
#include "sdf/Model.hh"
#include "sdf/Root.hh"
#include "sdf/Types.hh"
#include "test_config.h"

//////////////////////////////////////////////////
TEST(DOMJoint, NotAJoint)
{
  // Create an Element that is not a joint
  sdf::ElementPtr element(new sdf::Element);
  element->SetName("world");
  sdf::Joint joint;
  std::shared_ptr<sdf::FrameGraph> frameGraph(new sdf::FrameGraph);
  sdf::Errors errors = joint.Load(element, frameGraph);
  ASSERT_FALSE(errors.empty());
  EXPECT_EQ(errors[0].Code(), sdf::ErrorCode::ELEMENT_INCORRECT_TYPE);
  EXPECT_TRUE(errors[0].Message().find("Attempting to load a Joint") !=
               std::string::npos);
}

//////////////////////////////////////////////////
TEST(DOMJoint, NoName)
{
  // Create a "joint" with no name
  sdf::ElementPtr element(new sdf::Element);
  element->SetName("joint");

  sdf::Joint joint;
  std::shared_ptr<sdf::FrameGraph> frameGraph(new sdf::FrameGraph);
  sdf::Errors errors = joint.Load(element, frameGraph);
  ASSERT_FALSE(errors.empty());
  EXPECT_EQ(errors[0].Code(), sdf::ErrorCode::ATTRIBUTE_MISSING);
  EXPECT_TRUE(errors[0].Message().find("joint name is required") !=
               std::string::npos);
}

//////////////////////////////////////////////////
TEST(DOMJoint, NoFrameGraph)
{
  // Create a "link"
  sdf::ElementPtr element(new sdf::Element);
  element->SetName("joint");
  element->AddAttribute("name", "string", "link", true, "name");
  element->AddAttribute("type", "string", "revolute", true, "type");

  sdf::Joint joint;
  sdf::Errors errors = joint.Load(element, nullptr);
  ASSERT_FALSE(errors.empty());

  bool found = false;
  sdf::ErrorCode code = sdf::ErrorCode::NONE;
  for (auto err : errors)
  {
    if (err.Message().find("frame graph is required") != std::string::npos)
    {
      found = true;
      code = err.Code();
      break;
    }
  }
  EXPECT_TRUE(found);
}

//////////////////////////////////////////////////
TEST(DOMJoint, DoublePendulum)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "double_pendulum.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first model
  const sdf::Model *model = root.ModelByIndex(0);
  ASSERT_NE(nullptr, model);

  // The double pendulum should have two joints.
  EXPECT_EQ(2u, model->JointCount());

  // Try to get an invalid joint by name
  EXPECT_TRUE(model->JointByName("invalid_joint") == nullptr);

  // Get the two joints
  const sdf::Joint *upperJoint = model->JointByName("upper_joint");
  ASSERT_NE(nullptr, upperJoint);
  const sdf::Joint *lowerJoint = model->JointByName("lower_joint");
  ASSERT_NE(nullptr, lowerJoint);

  // Check the parent and child link values
  EXPECT_EQ("base", upperJoint->ParentLinkName());
  EXPECT_EQ("upper_link", upperJoint->ChildLinkName());
  EXPECT_EQ("upper_link", lowerJoint->ParentLinkName());
  EXPECT_EQ("lower_link", lowerJoint->ChildLinkName());

  // The two joinst should not have a second axis.
  EXPECT_TRUE(upperJoint->Axis(1) == nullptr);
  EXPECT_TRUE(upperJoint->Axis(2) == nullptr);
  EXPECT_TRUE(lowerJoint->Axis(1) == nullptr);
  EXPECT_TRUE(lowerJoint->Axis(2) == nullptr);

  // Get the first axis for each joint
  const sdf::JointAxis *upperAxis = upperJoint->Axis(0);
  ASSERT_NE(nullptr, upperAxis);
  const sdf::JointAxis *lowerAxis = upperJoint->Axis(0);
  ASSERT_NE(nullptr, lowerAxis);

  // Check the xyz values for both axis.
  EXPECT_EQ(ignition::math::Vector3d::UnitX, upperAxis->Xyz());
  EXPECT_EQ(ignition::math::Vector3d::UnitX, lowerAxis->Xyz());
}
