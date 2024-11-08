#include "PerlinAlgorithm.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
   auto result = perlin::random3DVector(42);
   for (const auto& x : result) {
      std::cout << x << " ";
   }
   std::cout << std::endl;
   return 0;
}