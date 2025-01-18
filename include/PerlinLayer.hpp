#ifndef PERLIN_LAYER_HPP
#define PERLIN_LAYER_HPP
#include "PerlinOOP.hpp"
#include <cmath>
#include <vector>

namespace perlin {
class PerlinLayer {
   // I do not save a reference to gradients. Managing such a reference makes it hard to reassign layers to new objects as the owners of the reference might get deleted. (Happened before and the vector got reset-> bug)
   public:
   PerlinLayer(unsigned sizeX, unsigned sizeY, unsigned chunkSize, double weight) : sizeX(sizeX), sizeY(sizeY), chunkSize(chunkSize), weight(weight) {
      result = matrix(sizeX, std::vector<double>(sizeY, 0.0));
   };

   // Move constructor
   PerlinLayer(PerlinLayer&& other) noexcept
      : sizeX(other.sizeX), sizeY(other.sizeY), chunkSize(other.chunkSize), weight(other.weight), result(std::move(other.result)) {}

   // Move assignment operator
   PerlinLayer& operator=(PerlinLayer&& other) noexcept {
      if (this != &other) {
         // sizeX and sizeY are const, already initialized by the constructor
         chunkSize = other.chunkSize;
         weight = other.weight;
         result = std::move(other.result); // Move the matrix
      }
      return *this;
   }
   // Disable copy semantics -- copying is very inefficient
   // compiler should complain if copying is done
   PerlinLayer(const PerlinLayer&) = delete;
   PerlinLayer& operator=(const PerlinLayer&) = delete;

   void fill(const std::vector<vec2d>& gradients);
   void changeWeight(const double newWeight);
   void changeChunkSize(const std::vector<vec2d>& gradients, const unsigned newChunkSize); // triggers recompute

   void accumulate(matrix& accumulator, const double weightFactor);

   double getWeight() {
      return weight;
   }

   private:
   const unsigned sizeX;
   const unsigned sizeY;
   unsigned chunkSize;
   double weight = 1.0;
   matrix result;

   double computeWithIndices(const std::vector<vec2d>& gradients, const unsigned x, const unsigned y, const int valBL, const int valBR, const int valTL, const int valTR);
   void fillChunk(const std::vector<vec2d>& gradients, const unsigned chunkX, const unsigned chunkY);
}; // End of PerlinLayer class
} // End of namespace "perlin"
#endif