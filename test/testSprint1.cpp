#include "PerlinAlgorithm.hpp"
#include <gtest/gtest.h>

//-----------------------------------------------------------------------------
TEST(Perlin_Random3DVector, Seeding)
/// SD: test if perlin::random3DVector's seeding works
{
   int seed = 42;
   auto result1 = perlin::random3DVector(seed);
   auto result2 = perlin::random3DVector(seed);

   ASSERT_DOUBLE_EQ(result1.at(0), result2.at(0));
   ASSERT_DOUBLE_EQ(result1.at(1), result2.at(1));
   ASSERT_DOUBLE_EQ(result1.at(2), result2.at(2));
}

TEST(Perlin_Random3DVector, Normalized)
/// SD: test if perlin::random3DVector returns result with l2-norm equal to 1
{
   auto result = perlin::random3DVector(631);
   double sos{0.0};
   for (size_t i = 0; i < 3; i++) {
      sos += result.at(i) * result.at(i);
   }
   ASSERT_DOUBLE_EQ(1.0, sos);
}
//-----------------------------------------------------------------------------