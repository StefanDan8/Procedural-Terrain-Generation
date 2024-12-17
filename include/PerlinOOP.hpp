#ifndef _PERLIN_OOP
#define _PERLIN_OOP
#include "PerlinAlgorithm.hpp"
#include <stdexcept>
#include <vector>

namespace perlin {

class PerlinLayer2D {
   public:
   PerlinLayer2D(const unsigned chunkSize, const unsigned numChunksX, const unsigned numChunksY, std::vector<vec2d>& gradients) : chunkSize(chunkSize), numChunksX(numChunksX), numChunksY(numChunksY), gradients(gradients) {}
   PerlinLayer2D(const unsigned chunkSize, const unsigned numChunksX, const unsigned numChunksY, std::vector<vec2d>& gradients, double weight) : chunkSize(chunkSize), numChunksX(numChunksX), numChunksY(numChunksY), gradients(gradients), weight(weight) {}

   double compute(const unsigned x, const unsigned y, const unsigned chunkX, const unsigned chunkY);

   void fillChunk(matrix& result, const unsigned chunkX, const unsigned chunkY);
   void fill(matrix& result);

   void setWeight(double w) {
      weight = w;
   }

   double getWeight() {
      return weight;
   }

   private:
   const unsigned chunkSize;
   const unsigned numChunksX;
   const unsigned numChunksY;
   double weight = 1;
   std::vector<vec2d>& gradients;
};

class PerlinNoise2D {
   public:
   PerlinNoise2D(const unsigned sizeX, const unsigned sizeY, std::vector<std::pair<unsigned, double>>& layerParams) : sizeX(sizeX), sizeY(sizeY) {
      gradients.resize(128);
      for (auto& grad : gradients) {
         grad = random2DGrad();
      }
      for (const auto& chunkSizeWeight : layerParams) {
         if (sizeX % chunkSizeWeight.first != 0 || sizeY % chunkSizeWeight.first != 0) {
            throw std::invalid_argument("The size of the terrain must be divisible by the chunk size.");
         }
      }
      for (const auto& chunkSizeWeight : layerParams) {
         auto chunkSize = chunkSizeWeight.first;
         auto weight = chunkSizeWeight.second;
         layers.push_back(PerlinLayer2D(chunkSize, sizeX / chunkSize, sizeY / chunkSize, gradients, weight));
      }
   }

   void fill(matrix& result);

   private:
   unsigned sizeX;
   unsigned sizeY;
   std::vector<PerlinLayer2D> layers;
   std::vector<vec2d> gradients;
};

}

#endif