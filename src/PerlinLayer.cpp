#include "PerlinLayer.hpp"
namespace perlin {

double PerlinLayer::computeWithIndices(const std::vector<vec2d>& gradients, const unsigned x, const unsigned y, const int valBL, const int valBR, const int valTL, const int valTR) {
   // Compute the position of the point within the square
   double dx = (x % chunkSize + 1) / (double) chunkSize;
   double dy = (y % chunkSize + 1) / (double) chunkSize;

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
   const unsigned offsetX = chunkSize * chunkX;
   const unsigned offsetY = chunkSize * chunkY;

   // Select a value for each of the 4 corners of the square from the permutation table
   auto size = gradients.size();
   // Here is PROBLEM gradients.size() is 0 here
   const int valBL = simpleHash(chunkX, chunkY, size);
   const int valBR = simpleHash(chunkX + 1, chunkY, size);
   const int valTL = simpleHash(chunkX, chunkY + 1, size);
   const int valTR = simpleHash(chunkX + 1, chunkY + 1, size);

   // if chunk does not fit entirely in matrix
   const unsigned boundX = std::min(offsetX + chunkSize, sizeX);
   const unsigned boundY = std::min(offsetY + chunkSize, sizeY);

   for (unsigned i = offsetX; i < boundX; i++) {
      for (unsigned j = offsetY; j < boundY; j++) {
         result[i][j] = computeWithIndices(gradients, i, j, valBL, valBR, valTL, valTR);
      }
   }
}

void PerlinLayer::fill(const std::vector<vec2d>& gradients) {
   const unsigned numChunksX = std::ceil(((double) sizeX) / chunkSize);
   const unsigned numChunksY = std::ceil(((double) sizeY) / chunkSize);
   for (unsigned chunkX = 0; chunkX < numChunksX; chunkX++) {
      for (unsigned chunkY = 0; chunkY < numChunksY; chunkY++) {
         fillChunk(gradients, chunkX, chunkY);
      }
   }
}

void PerlinLayer::changeWeight(const double newWeight) {
   weight = newWeight;
}

/// @brief Adjusts chunkSize and triggers recomputation of the layer
/// @note `gradients` is added also as a sign of recomputation
/// @param gradients
/// @param newChunkSize
void PerlinLayer::changeChunkSize(const std::vector<vec2d>& gradients, const unsigned newChunkSize) {
   chunkSize = newChunkSize;
   fill(gradients);
}

void PerlinLayer::accumulate(matrix& accumulator, const double weightFactor) {
   std::cout << "Accumulate called with weight " << weightFactor << std::endl;
   for (unsigned i = 0; i < accumulator.size(); ++i) {
      for (unsigned j = 0; j < accumulator[0].size(); ++j) {
         accumulator[i][j] += weightFactor * result[i][j];
      }
   }
}

}