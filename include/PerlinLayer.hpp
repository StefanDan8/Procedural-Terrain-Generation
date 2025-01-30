#ifndef PERLIN_LAYER_HPP
#define PERLIN_LAYER_HPP

#include "PerlinUtils.hpp"

enum UpdateState {
   NONE, // 0
   WEIGHT, // 1
   CHUNK_SIZE, // 2
   BOTH // 3
};

enum LayerType {
   NOISE_LAYER,
   BASELINE_LAYER
};

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

   // --- Methods ---

   /// @brief Fill the entire result matrix with Perlin noise values
   /// @param gradients Constant gradients used for computation
   void fill(const std::vector<vec2d>& gradients);

   void changeWeight(const double newWeight);

   /// @note Triggers recompute
   void changeChunkSize(const std::vector<vec2d>& gradients, const unsigned newChunkSize);

   /// @brief Add the values of the layer to the accumulator matrix
   /// @param accumulator the matrix to accumulate the values to
   /// @param weightFactor the factor to multiply the values with
   void accumulate(matrix& accumulator, const double weightFactor);

   double getWeight() {
      return weight;
   }

   unsigned getChunkSize() {
      return chunkSize;
   }

   private:
   const unsigned sizeX;
   const unsigned sizeY;
   unsigned chunkSize;
   double weight = 1.0;
   matrix result;

   /// @brief Compute the Perlin noise value of a pixel
   /// @param gradients Constant gradients used for computation
   /// @param x x-coordinate within the chunk
   /// @param y y-coordinate within the chunk
   /// @param valBL index of bottom left gradient
   /// @param valBR index of bottom right gradient
   /// @param valTL index of top left gradient
   /// @param valTR index of top right gradient
   double computeWithIndices(const std::vector<vec2d>& gradients, const unsigned x, const unsigned y, const int valBL, const int valBR, const int valTL, const int valTR);

   /// @brief Fill a chunk of the matrix with Perlin noise values
   /// @param gradients Constant gradients used for computation
   /// @param chunkX x-coordinate of the chunk within the entire grid
   /// @param chunkY y-coordinate of the chunk within the entire grid
   void fillChunk(const std::vector<vec2d>& gradients, const unsigned chunkX, const unsigned chunkY);

}; // End of PerlinLayer class
} // End of namespace "perlin"
#endif // PERLIN_LAYER_HPP