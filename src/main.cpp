#include "PerlinAlgorithm.hpp"
#include "Renderer.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
   const unsigned chunkSize = 16;
   const unsigned nChunksX = 16;
   const unsigned nChunksY = 16;
   perlin::initialize2DGradients(chunkSize);
   perlin::matrix result(nChunksX * chunkSize, std::vector<double>(nChunksY * chunkSize, 0.0));
   const std::vector<unsigned> permutationTable = perlin::generatePermutationTable(42, chunkSize);
   for (unsigned i = 0; i < nChunksX; i++) {
      for (unsigned j = 0; j < nChunksY; j++) {
         perlin::fill2DChunk(result, i, j, permutationTable);
      }
   }

   // render::writeMatrixToFile(render::normalizeMatrix(result), "output16x16x16.txt");

   // auto permutationTable = perlin::generatePermutationTable(42);
   // auto noise = perlin::Noise2D(5.0, 7.0, permutationTable);
   // std::cout << noise << std::endl;

   std::vector<double> noise2DExample = {-1.0, -0.8, -0.4, -0.2, 0.0,
                                         0.2, 0.4, 0.6, 0.8, 1};
   render::create_png(render::normalizeMatrix(result));

   return 0;
}