#include <gtest/gtest.h>
#include <core/utils/utils.h>

namespace atom {

constexpr char DELIM = ':';

TEST(SplitStringTest, SplitEmptyString)
{
  ASSERT_EQ(1, split_string("", DELIM).size());
}

TEST(SplitStringTest, SplitSimpleString)
{
  ASSERT_EQ(1, split_string("hello", DELIM).size());
  ASSERT_EQ("hello", split_string("hello", DELIM)[0]);
}

TEST(SplitStringTest, SplitDelimEndedString)
{
  ASSERT_EQ(2, split_string("Hello:", DELIM).size());
  ASSERT_EQ("Hello", split_string("Hello:", DELIM)[0]);
  ASSERT_EQ("", split_string("Hello:", DELIM)[1]);
}


TEST(SplitStringTest, SplitComplexString)
{
  StringArray tokens = split_string("Hello:Foo:Hi:Aloha", DELIM);
  ASSERT_EQ(4, tokens.size());
  ASSERT_EQ("Hello", tokens[0]);
  ASSERT_EQ("Foo", tokens[1]);
  ASSERT_EQ("Hi", tokens[2]);
  ASSERT_EQ("Aloha", tokens[3]);
}

TEST(SplitStringTest, SplitComplexDelimEndedString)
{
  StringArray tokens = split_string("Hello:Foo:Hi:", DELIM);
  ASSERT_EQ(4, tokens.size());
  ASSERT_EQ("Hello", tokens[0]);
  ASSERT_EQ("Foo", tokens[1]);
  ASSERT_EQ("Hi", tokens[2]);
  ASSERT_EQ("", tokens[3]);
}

}
