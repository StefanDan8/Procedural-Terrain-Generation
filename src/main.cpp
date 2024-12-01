#include "PerlinAlgorithm.hpp"
#include "Renderer.hpp"
#include "UserInterface.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
   const unsigned chunkSize = gui::get_positive_number("Please enter the chunk size (> 0): ");
   const unsigned nChunksX = gui::get_positive_number("Please enter the number of chunks in the x direction (> 0): ");
   const unsigned nChunksY = gui::get_positive_number("Please enter the number of chunks in the y direction (> 0): ");

   perlin::initialize2DGradients(chunkSize);
   perlin::matrix result(nChunksX * chunkSize, std::vector<double>(nChunksY * chunkSize, 0.0));

   const std::vector<unsigned> permutationTable = perlin::generatePermutationTable(42, chunkSize);
   for (unsigned i = 0; i < nChunksX; i++) {
      for (unsigned j = 0; j < nChunksY; j++) {
         perlin::fill2DChunk(result, i, j, permutationTable);
      }
   }

   // render::writeMatrixToFile(render::normalizeMatrix(result), "output16x16x16.txt");

   render::create_png(render::normalizeMatrix(result));

   return 0;
}