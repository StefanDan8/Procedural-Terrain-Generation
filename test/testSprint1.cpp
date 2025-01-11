#include "PerlinAlgorithm.hpp"
#include <gtest/gtest.h>

#define ASSERT_DOUBLE_NE(val1, val2) ASSERT_PRED_FORMAT2( \
   !::testing::internal::CmpHelperFloatingPointEQ<double>, val1, val2)

//-----------------------------------------------------------------------------

TEST(Perlin_Random3DGrad, Normalized)
/// SD: test if perlin::random3DGrad returns result with l2-norm equal to 1
{
   perlin::UniformUnitGenerator unif = perlin::UniformUnitGenerator(631);
   auto result = perlin::random3DGrad(unif);
   double sos{0.0};
   for (size_t i = 0; i < 3; i++) {
      sos += result.at(i) * result.at(i);
   }
   ASSERT_DOUBLE_EQ(1.0, sos);
}

TEST(Perlin_Random3DGrad, NonRepetitive)
/// SD: test if perlin::random3DGrad returns different results for consecutive use with same generator
{
   perlin::UniformUnitGenerator unif = perlin::UniformUnitGenerator(631);
   auto result1 = perlin::random3DGrad(unif);
   auto result2 = perlin::random3DGrad(unif);
   ASSERT_DOUBLE_NE(result1.at(0), result2.at(0));
   ASSERT_DOUBLE_NE(result1.at(1), result2.at(1));
   ASSERT_DOUBLE_NE(result1.at(2), result2.at(2));
}

TEST(Perlin_Random2DGrad, Normalized)
/// SD: test if perlin::random2DGrad returns result with l2-norm equal to 1
{
   perlin::UniformUnitGenerator unif = perlin::UniformUnitGenerator(631);
   auto result = perlin::random2DGrad(unif);
   double sos{0.0};
   for (size_t i = 0; i < 2; i++) {
      sos += result.at(i) * result.at(i);
   }
   ASSERT_DOUBLE_EQ(1.0, sos);
}

TEST(Perlin_Random2DGrad, NonRepetitive)
/// SD: test if perlin::random3DGrad returns different results for consecutive use with same generator
{
   perlin::UniformUnitGenerator unif = perlin::UniformUnitGenerator(631);
   auto result1 = perlin::random2DGrad(unif);
   auto result2 = perlin::random2DGrad(unif);
   ASSERT_DOUBLE_NE(result1.at(0), result2.at(0));
   ASSERT_DOUBLE_NE(result1.at(1), result2.at(1));
}
//-----------------------------------------------------------------------------