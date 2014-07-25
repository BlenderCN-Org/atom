#include <core/lru_cache.h>
#include <gtest/gtest.h>
#include <random>
#include <limits>
#include <array>

using namespace atom;
using namespace std;

const int MAX_SIZE = 100;

class LRUCacheTestInt : public testing::Test {
public:
  LRUCacheTestInt()
    : my_cache(MAX_SIZE)
    , my_dist(0, numeric_limits<int>::max())
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
  uniform_int_distribution<int> my_dist;
  mt19937 my_gen;
};

TEST_F(LRUCacheTestInt, Emptyness)
{
  ASSERT_EQ(0, my_cache.size());

  for (int i = 0; i < 1000; ++i) {
    auto iter = my_cache.find(random_int());
    EXPECT_TRUE(end(my_cache) == iter);
    EXPECT_TRUE(begin(my_cache) == iter);
    EXPECT_EQ(0, my_cache.size());
  }
}

TEST_F(LRUCacheTestInt, Fullness)
{
  ASSERT_EQ(0, my_cache.size());

  my_cache.insert(1, 1);
  EXPECT_EQ(1, my_cache.size());
  EXPECT_TRUE(begin(my_cache) == my_cache.find(1));
  EXPECT_FALSE(end(my_cache) == my_cache.find(1));

  // napln cache samymi 5
  for (int i = 0; i < 10 * MAX_SIZE; ++i) {
    EXPECT_FALSE(end(my_cache) == my_cache.insert(random_int(), 5));
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
      EXPECT_FALSE(end(my_cache) == my_cache.find(key));
    else
      EXPECT_TRUE(end(my_cache) == my_cache.find(key));
  }

  array<int, 10> test_keys;
  array<int, 10> test_values;

  for (int i = 0; i < test_keys.size(); ++i) {
    test_keys[i] = random_int();
    test_values[i] = random_int();

    EXPECT_TRUE(end(my_cache) != my_cache.insert(test_keys[i], test_values[i]));
  }

  ASSERT_EQ(MAX_SIZE, my_cache.size());

  // over vlozene hodnoty
  for (int i = 0; i < test_keys.size(); ++i) {
    ASSERT_TRUE(end(my_cache) != my_cache.find(test_keys[i]));
    EXPECT_TRUE(test_values[i] == *my_cache.find(test_keys[i]));
  }

  ASSERT_FALSE(begin(my_cache) == end(my_cache));
}

TEST_F(LRUCacheTestInt, IteratorTest)
{
  ASSERT_EQ(0, my_cache.size());

  for (int i = 0; i < 1000; ++i) {
    auto iter = my_cache.find(random_int());
    EXPECT_TRUE(end(my_cache) == iter);
    EXPECT_TRUE(begin(my_cache) == iter);
    EXPECT_EQ(0, my_cache.size());
  }

  my_cache.clear();

  ASSERT_TRUE(begin(my_cache) == end(my_cache));
}
