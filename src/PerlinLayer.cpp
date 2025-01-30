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

   // --- New implementation: potentially executing the 4 computations in parallel

   // // Compute local coordinates within the grid cell
   //  double dx = (x % chunkSize + 1) / static_cast<double>(chunkSize);
   //  double dy = (y % chunkSize + 1) / static_cast<double>(chunkSize);

   //  // Define relative vectors
   //  std::array<vec2d, 4> positions = {
   //      vec2d{dx, dy},         // BL
   //      vec2d{dx - 1.0, dy},   // BR
   //      vec2d{dx, dy - 1.0},   // TL
   //      vec2d{dx - 1.0, dy - 1.0} // TR
   //  };

   //  // Compute dot products in parallel
   //  std::array<double, 4> dotResults;
   //  std::array<int, 4> indices = {valBL, valBR, valTL, valTR};

   //  std::transform(std::execution::par, indices.begin(), indices.end(), positions.begin(), dotResults.begin(), [&](int idx, const vec2d& pos) {
   //                     return dot(gradients.at(idx), pos);
   //                 });

   //  // Compute fade functions
   //  double u = fade(dx);
   //  double v = fade(dy);

   //  // Bilinear interpolation
   //  return lerp(lerp(dotResults[0], dotResults[1], u),
   //              lerp(dotResults[2], dotResults[3], u), v);
}

void PerlinLayer::fillChunk(const std::vector<vec2d>& gradients, const unsigned chunkX, const unsigned chunkY) {
   if (gradients.empty()) return; // Prevent out-of-bounds access - should not happen

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

   // --- sequential loop
   for (unsigned i = offsetX; i < boundX; i++) {
      for (unsigned j = offsetY; j < boundY; j++) {
         result[i][j] = computeWithIndices(gradients, i, j, valBL, valBR, valTL, valTR);
      }
   }

   // --- parallel loop
   // Parallel loop over grid cells
   // std::for_each(std::execution::par, offsetX, boundX, [&](unsigned i) {
   //    std::for_each(std::execution::par, offsetY, boundY, [&](unsigned j) {
   //       result[i][j] = computeWithIndices(gradients, i, j, valBL, valBR, valTL, valTR);
   //    });
   // });

   // std::for_each(std::execution::par, result.begin(), result.end(), [this, &gradients, valBL, valBR, valTL, valTR](auto& row){
   //    unsigned i = &row - &result[0];  // Compute row index from the reference
   //    std::for_each(std::execution::par, result[i].begin(), result[i].end(), 
   //          [this, &gradients, valBL, valBR, valTL, valTR, i](auto& cell) {
   //              unsigned j = &cell - &result[i][0];  // Compute column index from the reference
   //              cell = computeWithIndices(gradients, i, j, valBL, valBR, valTL, valTR);
   //          });
   // });
}

void PerlinLayer::fill(const std::vector<vec2d>& gradients) {
   std::cout << "Fill called with on a layer with dimensions " << sizeX << " x " << sizeY << " and chunk size " << chunkSize;
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

   // --- parallel loop - doesn't work
   // Parallel loop over grid cells
   // std::for_each(std::execution::par, 0u, numChunksX, [&](unsigned chunkX) {
   //    std::for_each(std::execution::par, 0u, numChunksY, [&](unsigned chunkY) {
   //       fillChunk(gradients, chunkX, chunkY);
   //    });
   // });

   // Measuring time
   auto end = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> elapsed = end - start;
   std::cout << ", duration: " << elapsed.count() << " seconds" << std::endl;
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
   std::cout << "Accumulate called with weight " << weightFactor;

   // Ensure accumulator and result have the same dimensions
    if (accumulator.empty() || accumulator.size() != result.size() || accumulator[0].size() != result[0].size()) {
        throw std::runtime_error("Dimension mismatch between accumulator and result.");
    }

   // measuring the time
   auto start = std::chrono::high_resolution_clock::now();

   // --- sequential loop
   // Accumulate the values of the layer to the accumulator matrix
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