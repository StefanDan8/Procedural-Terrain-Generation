#include "PerlinOOP.hpp"
namespace perlin {
double PerlinLayer2D::compute(const unsigned x, const unsigned y, const unsigned chunkX, const unsigned chunkY) {
   double dx = (x % chunkSize + 1) / (double) chunkSize;
   double dy = (y % chunkSize + 1) / (double) chunkSize;

   vec2d BL = {dx, dy};
   vec2d BR = {dx - 1.0, dy};
   vec2d TL = {dx, dy - 1.0};
   vec2d TR = {dx - 1.0, dy - 1.0};

   auto size = gradients.size();
   int valBL = simpleHash(chunkX, chunkY, size);
   int valBR = simpleHash(chunkX + 1, chunkY, size);
   int valTL = simpleHash(chunkX, chunkY + 1, size);
   int valTR = simpleHash(chunkX + 1, chunkY + 1, size);

   double dotBL = dot(gradients.at(valBL), BL);
   double dotBR = dot(gradients.at(valBR), BR);
   double dotTL = dot(gradients.at(valTL), TL);
   double dotTR = dot(gradients.at(valTR), TR);

   // Compute fade curves for x and y
   double u = fade(dx);
   double v = fade(dy);
   // double u = dx;
   // double v = dy;

   // Interpolate the 4 results
   return (lerp(lerp(dotBL, dotBR, u), lerp(dotTL, dotTR, u), v)) * weight;
}
void PerlinLayer2D::fillChunk(matrix& result, const unsigned chunkX, const unsigned chunkY) {
   const unsigned offsetX = chunkSize * chunkX;
   const unsigned offsetY = chunkSize * chunkY;
   for (unsigned i = offsetX; i < offsetX + chunkSize; i++) {
      for (unsigned j = offsetY; j < offsetY + chunkSize; j++) {
         result[i][j] += compute(i, j, chunkX, chunkY);
      }
   }
}

void PerlinLayer2D::fill(matrix& result) {
   for (unsigned i = 0; i < numChunksX; i++) {
      for (unsigned j = 0; j < numChunksY; j++) {
         fillChunk(result, i, j);
      }
   }
}

void PerlinNoise2D::fill(matrix& result) {
   for (auto& layer : layers) {
      layer.fill(result);
   }
}
}