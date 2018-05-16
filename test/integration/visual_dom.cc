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
#include "sdf/Error.hh"
#include "sdf/Filesystem.hh"
#include "sdf/Link.hh"
#include "sdf/Model.hh"
#include "sdf/Root.hh"
#include "sdf/Types.hh"
#include "sdf/Visual.hh"
#include "test_config.h"

//////////////////////////////////////////////////
TEST(DOMVisual, NotAVisual)
{
  // Create an Element that is not a visual
  sdf::ElementPtr element(new sdf::Element);
  element->SetName("world");
  sdf::Visual visual;
  sdf::Errors errors = visual.Load(element);
  ASSERT_FALSE(errors.empty());
  EXPECT_EQ(errors[0].Code(), sdf::ErrorCode::ELEMENT_INCORRECT_TYPE);
  EXPECT_TRUE(errors[0].Message().find("Attempting to load a Visual") !=
               std::string::npos);
}

//////////////////////////////////////////////////
TEST(DOMVisual, NoName)
{
  // Create a "visual" with no name
  sdf::ElementPtr element(new sdf::Element);
  element->SetName("visual");

  element->PrintValues("  ");
  sdf::Visual visual;
  sdf::Errors errors = visual.Load(element);
  ASSERT_FALSE(errors.empty());
  EXPECT_EQ(errors[0].Code(), sdf::ErrorCode::ATTRIBUTE_MISSING);
  EXPECT_TRUE(errors[0].Message().find("visual name is required") !=
               std::string::npos);
}

//////////////////////////////////////////////////
TEST(DOMVisual, DoublePendulum)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "double_pendulum.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  const sdf::Model *model = root.ModelByIndex(0);
  ASSERT_TRUE(model != nullptr);

  const sdf::Link *baseLink = model->LinkByIndex(0);
  ASSERT_TRUE(baseLink != nullptr);

  const sdf::Visual *plateVis = baseLink->VisualByIndex(0);
  ASSERT_TRUE(plateVis != nullptr);

  EXPECT_EQ(ignition::math::Pose3d(0, 0, 0.01, 0, 0, 0), plateVis->Pose());
  EXPECT_EQ("", plateVis->PoseFrame());

  const sdf::Visual *poleVis = baseLink->VisualByIndex(1);
  ASSERT_TRUE(poleVis != nullptr);

  EXPECT_EQ(ignition::math::Pose3d(-0.275, 0, 1.1, 0, 0, 0), poleVis->Pose());
  EXPECT_EQ("", poleVis->PoseFrame());
}