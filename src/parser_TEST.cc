/*
 * Copyright 2017 Open Source Robotics Foundation
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

#include <gtest/gtest.h>
#include "sdf/parser.hh"
#include "sdf/Element.hh"
#include "test_config.h"

/////////////////////////////////////////////////
TEST(parser, initStringTrim)
{
  sdf::SDFPtr sdf(new sdf::SDF());
  std::ostringstream stream;
  stream << "<element name=\"visual\" required=\"*\" "
         <<          "type=\"string\" default=\"_default_value_\">"
         << "  <attribute name=\"name\" type=\"string\" required=\" 1\""
         << "             default=\"__default__\">"
         << "    <description>test</description>"
         << "  </attribute>"
         << "</element>";

  EXPECT_TRUE(sdf::initString(stream.str(), sdf));
  sdf::ElementPtr root = sdf->Root();
  EXPECT_TRUE(root != nullptr);

  EXPECT_EQ("visual", root->GetName());
  EXPECT_EQ("*", root->GetRequired());
  sdf::ParamPtr value = root->GetValue();
  ASSERT_NE(nullptr, value);
  EXPECT_EQ("string", value->GetTypeName());
  EXPECT_EQ("_default_value_", value->GetDefaultAsString());

  sdf::ParamPtr attr = root->GetAttribute("name");
  EXPECT_TRUE(attr != nullptr);
  EXPECT_TRUE(attr->GetRequired());
}

/////////////////////////////////////////////////
/// Check that _a contains _b
static bool contains(const std::string &_a, const std::string &_b)
{
  return _a.find(_b) != std::string::npos;
}

/////////////////////////////////////////////////
TEST(Parser, SyntaxErrorInValues)
{
  std::string pathBase = PROJECT_SOURCE_PATH;
  pathBase += "/test/sdf";

  // Capture sdferr output
  std::stringstream buffer;
  auto old = std::cerr.rdbuf(buffer.rdbuf());

  {
    std::string path = pathBase +"/bad_syntax_pose.sdf";
    sdf::SDFPtr sdf(new sdf::SDF());
    sdf::init(sdf);

    sdf::readFile(path, sdf);
    EXPECT_PRED2(contains, buffer.str(),
                 "Unable to set value [bad 0 0 0 0 0 ] for key[pose]");
  }
  {
    // clear the contents of the buffer
    buffer.str("");
    std::string path = pathBase +"/bad_syntax_double.sdf";
    sdf::SDFPtr sdf(new sdf::SDF());
    sdf::init(sdf);

    sdf::readFile(path, sdf);
    EXPECT_PRED2(contains, buffer.str(),
                 "Unable to set value [bad ] for key[linear]");
  }
  {
    // clear the contents of the buffer
    buffer.str("");
    std::string path = pathBase +"/bad_syntax_vector.sdf";
    sdf::SDFPtr sdf(new sdf::SDF());
    sdf::init(sdf);

    sdf::readFile(path, sdf);
    EXPECT_PRED2(contains, buffer.str(),
                 "Unable to set value [0 1 bad ] for key[gravity]");
  }

  // Revert cerr rdbug so as to not interfere with other tests
  std::cerr.rdbuf(old);
}

/////////////////////////////////////////////////
/// Main
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
