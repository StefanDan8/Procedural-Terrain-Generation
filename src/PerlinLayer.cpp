#include "PerlinLayer.hpp"
namespace perlin {

double PerlinLayer::computeWithIndices(const std::vector<vec2d>& gradients, const unsigned x, const unsigned y, const int valBL, const int valBR, const int valTL, const int valTR) {
   // Compute the position of the point within the square
   double dx = (x % chunkSize + 1) / static_cast<double>(chunkSize);
   double dy = (y % chunkSize + 1) / static_cast<double>(chunkSize);

   // Find the vectors pointing to the point from the 4 corners of the square
   const vec2d BL = {dx, dy};
   const vec2d BR = {dx - 1.0, dy};
   const vec2d TL = {dx, dy - 1.0};
   const vec2d TR = {dx - 1.0, dy - 1.0};

   // Compute the corresponding dot products
   double dotBL = dot(gradients.at(valBL), BL);
   double dotBR = dot(gradients.at(valBR), BR);
   double dotTL = dot(gradients.at(valTL), TL);
   double dotTR = dot(gradients.at(valTR), TR);

   // Compute fade curves for x and y
   double u = fade(dx);
   double v = fade(dy);

   // Interpolate the 4 results
   return (lerp(lerp(dotBL, dotBR, u), lerp(dotTL, dotTR, u), v));
}

void PerlinLayer::fillChunk(const std::vector<vec2d>& gradients, const unsigned chunkX, const unsigned chunkY) {
   if (gradients.empty()) return; // Prevent out-of-bounds access - should not happen

   const unsigned offsetX = chunkSize * chunkX;
   const unsigned offsetY = chunkSize * chunkY;

   size_t size = gradients.size();

   // Select a value for each of the 4 corners of the square from the permutation table
   const int valBL = simpleHash(chunkX, chunkY, size);
   const int valBR = simpleHash(chunkX + 1, chunkY, size);
   const int valTL = simpleHash(chunkX, chunkY + 1, size);
   const int valTR = simpleHash(chunkX + 1, chunkY + 1, size);

   // if chunk does not fit entirely in matrix
   const unsigned boundX = std::min(offsetX + chunkSize, sizeX);
   const unsigned boundY = std::min(offsetY + chunkSize, sizeY);

   // --- sequential loop
   for (unsigned i = offsetX; i < boundX; i++) {
      for (unsigned j = offsetY; j < boundY; j++) {
         result[i][j] = computeWithIndices(gradients, i, j, valBL, valBR, valTL, valTR);
      }
   }
}

void PerlinLayer::fill(const std::vector<vec2d>& gradients) {
   std::cout << "Fill called on a layer with dimensions " << sizeX << " x " << sizeY << " and chunk size " << chunkSize;
   const unsigned numChunksX = std::ceil(((double) sizeX) / chunkSize);
   const unsigned numChunksY = std::ceil(((double) sizeY) / chunkSize);

   // Measuring time
   auto start = std::chrono::high_resolution_clock::now();

   // --- sequential loop
   for (unsigned chunkX = 0; chunkX < numChunksX; chunkX++) {
      for (unsigned chunkY = 0; chunkY < numChunksY; chunkY++) {
         fillChunk(gradients, chunkX, chunkY);
      }
   }

   // Measuring time
   auto end = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> elapsed = end - start;
   std::cout << ", duration: " << elapsed.count() << " seconds" << std::endl;
}

void PerlinLayer::changeWeight(const double newWeight) {
   weight = newWeight;
}

void PerlinLayer::changeChunkSize(const std::vector<vec2d>& gradients, const unsigned newChunkSize) {
   chunkSize = newChunkSize;
   fill(gradients);
}

void PerlinLayer::accumulate(matrix& accumulator, const double weightFactor) {
   std::cout << "Accumulate called with weight " << weightFactor;

   // Ensure accumulator and result have the same dimensions
   if (accumulator.empty() || accumulator.size() != result.size() || accumulator[0].size() != result[0].size()) {
      throw std::runtime_error("Dimension mismatch between accumulator and result.");
   }

   // measuring the time
   auto start = std::chrono::high_resolution_clock::now();

   // --- sequential loop
   // // Accumulate the values of the layer to the accumulator matrix
   // for (unsigned i = 0; i < accumulator.size(); ++i) {
   //    for (unsigned j = 0; j < accumulator[0].size(); ++j) {
   //       accumulator[i][j] += weightFactor * result[i][j];
   //    }
   // }

   // --- parallel loop
   // Parallel transformation for each row
   std::transform(std::execution::par, accumulator.begin(), accumulator.end(),
                  result.begin(), accumulator.begin(),
                  [weightFactor](std::vector<double>& accRow, const std::vector<double>& resRow) {
                     std::transform(std::execution::par, accRow.begin(), accRow.end(),
                                    resRow.begin(), accRow.begin(),
                                    [weightFactor](double accVal, double resVal) {
                                       return accVal + weightFactor * resVal;
                                    });
                     return accRow;
                  });

   // measuring the time
   auto end = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> elapsed = end - start;
   std::cout << ", duration: " << elapsed.count() << " seconds" << std::endl;
}

} // namespace perlin