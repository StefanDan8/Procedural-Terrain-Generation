#ifndef _PERLIN_OOP
#define _PERLIN_OOP

#include "PerlinLayer.hpp"

namespace perlin {

// ----- Noise Functions -----

class PerlinNoise2D {
   private:
   // --- Class Parameters ---
   double weightSum = 0.0; // The sum of the weights of the layers
   std::vector<vec2d> gradients; // The constant gradients used for computation
   matrix resultMatrix; // The matrix to fill
   std::vector<PerlinLayer> layers; // The layers of the noise
   unsigned sizeX; // The size of the terrain in the x direction
   unsigned sizeY; // The size of the terrain in the y direction

   public:

   /// @brief Noise constructor, initializes the gradients and the layers
   /// @param sizeX The size of the terrain in the x direction
   /// @param sizeY The size of the terrain in the y direction
   /// @param layerParams A vector of pairs, each pair containing the chunk size and the weight of the layer
   PerlinNoise2D(unsigned sizeX, unsigned sizeY, const std::vector<std::pair<unsigned, double>>& layerParams);

   // --- Matrix functions ---

   /// @brief Get the copy of the result matrix
   matrix getResult() {
      return resultMatrix;
   }

   /// @brief Get the reference to the result matrix
   const matrix& getResultRef() {
      return resultMatrix;
   }

   /// @brief Initialize the matrix with zeros again
   void resetMatrix();

   /// @brief Resize the matrix to the new size and fill the new entries with 0, while keeping the original values at original positions
   void resizeMatrix(unsigned newSizeX, unsigned newSizeY);

   /// @brief Fill the whole matrix with Perlin noise values
   void fill();

   /// @brief Return the minimum and maximum values in the matrix
   std::pair<double, double> getMinMaxVal();

   /// @brief Normalize the matrix to the range [0, 255]
   void normalizeMatrix0255();

   /// @brief Normalize the matrix to the range [-1, 1]
   void normalizeMatrixPM1();

   /// @brief Normalize the matrix by dividing by the sum of the weights
   void normalizeMatrixSUM(const double flatteningFactor = 1.0f);

   /// @brief Normalize the matrix to the range [0, 255] and apply the ReLU function with minimal threshold to the matrix
   /// @param threshold the threshold for the ReLU function
   void normalizeMatrixReLU(const double threshold = 0.0f);

   /// @brief Apply the ReLU function with minimal threshold to the matrix
   /// @param threshold the threshold for the ReLU function
   void matrixReLU(const double threshold = 0.0f);

   /// @brief Update the own matrix with the maximum values of the own and another PerlinNoise2D object's matrix
   /// @param other another PerlinNoise2D object
   void filterMatrix(perlin::PerlinNoise2D& other);

   // --- Layer functions ---

   // Disable copy semantics -- copying is very inefficient
   // compiler should complain if copying is done
   /// @brief Set the layers of the noise with the already computed layers
   // void setLayers(std::vector<PerlinLayer>& newLayers);

   /// @brief Set the layers of the noise with the parameters of the layers
   void setLayers(std::vector<std::pair<unsigned, double>>& newLayerParams);

   /// @brief Add a new layer to the noise
   void addLayer(std::pair<unsigned, double>& newLayer);

   /// @brief Remove the last added layer
   void removeLayer();

   /// @brief Remove a layer at a specific index
   void removeLayer(unsigned index);

   /// @brief Recompute a layer at a specific index with new parameters
   void recomputeLayer(unsigned index, const std::pair<unsigned, double>& layerParams);

   /// @brief Recompute a layer at a specific index with new parameters
   void recomputeLayer(unsigned index, unsigned chunkSize, double weight);

   /// @brief Recompute a layer at a specific index with new chunkSize
   void recomputeLayerChunkSize(unsigned index, unsigned chunkSize);

   /// @brief Recompute a layer at a specific index with new weight
   void recomputeLayerWeight(unsigned index, double weight);

   /// @brief Recompute the weight sum of the layers
   /// @note Should not be used in the future, but rather update the weight sum when adding or removing layers
   void updateWeightSum();

   /// @note The check is added for test purposes, if the code works properly (the weight sum is updated correctly by each function involved) the check should be REMOVED - for performance reasons
   double getWeightSum() {
      // double tmp = weightSum;
      // updateWeightSum();
      // if (tmp != weightSum) {
      //    throw std::logic_error("Weight sum was not measured correctly!");
      // }
      return weightSum;
   }
}; // class PerlinNoise2D

} // namespace perlin

#endif // _PERLIN_OOP