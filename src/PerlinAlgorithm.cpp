#include "PerlinAlgorithm.hpp"
#include <random>
//-----------------------------------------------------------------------------
namespace perlin {
/// @author SD
/// @brief Generates a normalized 3D vector with non-negative values.
/// @param seed
/// @return
/// @note This is just a test function. Not to be used in production,
std::array<double, 3> random3DVector(int seed) {
   std::mt19937 generator(seed);
   std::uniform_real_distribution<double> unif(0.0, 1.0);
   auto x = unif(generator);
   auto y = unif(generator);
   auto z = unif(generator);
   auto length = sqrt(x * x + y * y + z * z);
   return std::array<double, 3>{x / length, y / length, z / length};
}
}