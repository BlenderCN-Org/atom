#include <core/lru_cache.h>
#include <core/platform.h>
#include <gtest/gtest.h>
#include <random>
#include <limits>
#include <array>

namespace atom {

/// @todo dopisat test s vyuzitim chytrych ukazovatelov (na test move semantiky)

const int MAX_SIZE = 100;

class LRUCacheTestSmart : public testing::Test {
public:
  LRUCacheTestSmart()
    : my_cache(MAX_SIZE)
    , my_dist(0, I32_MAX)
  {}

  int random_key()
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

TEST_F(LRUCacheTestSmart, Emptyness)
{
  ASSERT_EQ(0, my_cache.size());
}

}
