#include "PerlinAlgorithm.hpp"
#include "Renderer.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
   auto result = perlin::random3DVector(42);
   for (const auto& x : result) {
      std::cout << x << " ";
   }
   std::cout << std::endl;

   auto permutationTable = perlin::generatePermutationTable(42);
   auto noise = perlin::Noise2D(5.0, 7.0, permutationTable);
   std::cout << noise << std::endl;

   std::vector<double> noise2DExample = { -1.0, -0.8, -0.4, -0.2, 0.0,
                                          0.2, 0.4, 0.6, 0.8, 1 };
   render::ppm(noise2DExample, 2, 5);

   return 0;
}