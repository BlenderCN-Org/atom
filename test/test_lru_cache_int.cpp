#include <core/lru_cache.h>
#include <core/platform.h>
#include <gtest/gtest.h>
#include <random>
#include <array>

namespace atom {

const int MAX_SIZE = 100;

class LRUCacheTestInt : public testing::Test {
public:
  LRUCacheTestInt()
    : my_cache(MAX_SIZE)
    , my_dist(0, I32_MAX)
  {}

  int random_int()
  { return my_dist(my_gen); }

protected:
  void setUp()
  {
    my_cache.clear();
  }

  LRUCache<int, int> my_cache;

private:
  std::uniform_int_distribution<i32> my_dist;
  std::mt19937 my_gen;
};

TEST_F(LRUCacheTestInt, Emptyness)
{
  ASSERT_EQ(0, my_cache.size());

  for (int i = 0; i < 1000; ++i) {
    auto iter = my_cache.find(random_int());
    EXPECT_TRUE(my_cache.end() == iter);
    EXPECT_TRUE(my_cache.begin() == iter);
    EXPECT_EQ(0, my_cache.size());
  }
}

TEST_F(LRUCacheTestInt, Fullness)
{
  ASSERT_EQ(0, my_cache.size());

  my_cache.insert(1, 1);
  EXPECT_EQ(1, my_cache.size());
  EXPECT_TRUE(my_cache.begin() == my_cache.find(1));
  EXPECT_FALSE(my_cache.end() == my_cache.find(1));

  // napln cache samymi 5
  for (int i = 0; i < 10 * MAX_SIZE; ++i) {
    EXPECT_FALSE(my_cache.end() == my_cache.insert(random_int(), 5));
    EXPECT_GT(MAX_SIZE + 1, my_cache.size());
  }

  // over ze vsetky hodnoty v cache sa rovnaju 5
  for (int num : my_cache) {
    EXPECT_EQ(5, num);
  }

  // over vyhladavanie
  for (int i = 0; i < 10 * MAX_SIZE; ++i) {
    int key = random_int();
    if (key < MAX_SIZE)
      EXPECT_FALSE(my_cache.end() == my_cache.find(key));
    else
      EXPECT_TRUE(my_cache.end() == my_cache.find(key));
  }

  std::array<int, 10> test_keys;
  std::array<int, 10> test_values;

  for (u32 i = 0; i < test_keys.size(); ++i) {
    test_keys[i] = random_int();
    test_values[i] = random_int();

    EXPECT_TRUE(my_cache.end() != my_cache.insert(test_keys[i], test_values[i]));
  }

  ASSERT_EQ(MAX_SIZE, my_cache.size());

  // over vlozene hodnoty
  for (u32 i = 0; i < test_keys.size(); ++i) {
    ASSERT_TRUE(my_cache.end() != my_cache.find(test_keys[i]));
    EXPECT_TRUE(test_values[i] == *my_cache.find(test_keys[i]));
  }

  ASSERT_FALSE(my_cache.begin() == my_cache.end());
}

TEST_F(LRUCacheTestInt, IteratorTest)
{
  ASSERT_EQ(0, my_cache.size());

  for (int i = 0; i < 1000; ++i) {
    auto iter = my_cache.find(random_int());
    EXPECT_TRUE(my_cache.end() == iter);
    EXPECT_TRUE(my_cache.begin() == iter);
    EXPECT_EQ(0, my_cache.size());
  }

  my_cache.clear();

  ASSERT_TRUE(my_cache.begin() == my_cache.end());
}

}
