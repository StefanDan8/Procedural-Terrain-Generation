#ifndef _PERLIN_OOP
#define _PERLIN_OOP
#include <stdexcept>
#include <vector>
#include <array>
#include <iomanip> // For std::fixed and std::setprecision
#include <iostream>
#include <random>
#include "AppConfig.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>

namespace perlin {

// ----- General Functions -----

/// @brief 2D normalized real vector
using vec2d = std::array<double, 2>;

/// @brief 3D normalized real vector
using vec3d = std::array<double, 3>;

/// @brief Matrix with real values.
using matrix = std::vector<std::vector<double>>;

/// @brief 3D tensor with real values.
using tensor = std::vector<std::vector<std::vector<double>>>;


/// @brief Serves as global random number generator for Unif[0,1]
extern UniformUnitGenerator unifGlbl;
/// @brief Gradients for 2D generation. Assumed to have length = chunkSize
extern std::vector<vec2d> gradients2D;

int simpleHash(int i, int j, int N);

vec2d random2DGrad();
vec3d random3DGrad();

/// @author SD
/// @brief Generates a random 2D normalized vector
/// @param generator A random number generator for Unif[0,1]
vec2d random2DGrad(UniformUnitGenerator& generator);

/// @author SD
/// @brief Generates a random 3D normalized vector
/// @param generator A random number generator for Unif[0,1]
vec3d random3DGrad(UniformUnitGenerator& generator);

double dot(vec2d& x, vec2d& y);

double dot(vec3d& x, vec3d& y);

double fade(const double t);

double lerp(const double a, const double b, const double t);

// ----- Layer Functions -----

class PerlinLayer2D {
   public:
   // constructors
   PerlinLayer2D(const unsigned chunkSize, const unsigned numChunksX, const unsigned numChunksY, std::vector<vec2d>& gradients) : chunkSize(chunkSize), numChunksX(numChunksX), numChunksY(numChunksY), gradients(gradients) {}

   PerlinLayer2D(const unsigned chunkSize, const unsigned numChunksX, const unsigned numChunksY, std::vector<vec2d>& gradients, double weight) : chunkSize(chunkSize), numChunksX(numChunksX), numChunksY(numChunksY), gradients(gradients), weight(weight) {}

   // --- Methods ---

   /// @brief Compute the Perlin noise value of a pixel
   /// @param x x-coordinate within the chunk
   /// @param y y-coordinate within the chunk
   /// @param chunkX x-coordinate of the chunk within the entire grid
   /// @param chunkY y-coordinate of the chunk within the entire grid
   double compute(const unsigned x, const unsigned y, const unsigned chunkX, const unsigned chunkY);

   /// @brief Compute the Perlin noise value of a pixel
   /// @param x x-coordinate within the chunk
   /// @param y y-coordinate within the chunk
   /// @param valBL index of bottom left gradient
   /// @param valBR index of bottom right gradient
   /// @param valTL index of top left gradient
   /// @param valTR index of top right gradient
   double computeWithIndices(const unsigned x, const unsigned y, const int valBL, const int valBR, const int valTL, const int valTR);

   /// @brief Fill a chunk of the matrix with Perlin noise values
   /// @param result The matrix to fill
   /// @param chunkX x-coordinate of the chunk within the entire grid
   /// @param chunkY y-coordinate of the chunk within the entire grid
   void fillChunk(matrix& result, const unsigned chunkX, const unsigned chunkY);

   /// @brief Fill the entire matrix with Perlin noise values
   /// @param result The matrix to fill
   void fillMatrix(matrix& result);

   void setWeight(double w) {
      weight = w;
   }

   double getWeight() {
      return weight;
   }

   unsigned getChunkSize() {
      return chunkSize;
   }

   PerlinLayer2D operator=(const PerlinLayer2D& other) {
      return PerlinLayer2D(other.chunkSize, other.numChunksX, other.numChunksY, other.gradients, other.weight);
   }

   private:
   // --- Class Parameters ---
   const unsigned chunkSize; // The size of the chunk
   const unsigned numChunksX; // The number of chunks in the x direction
   const unsigned numChunksY; // The number of chunks in the y direction
   std::vector<vec2d>& gradients; // The gradients used for computation
   double weight = 1.0; // The weight of the layer
};

// ----- Noise Functions -----

class PerlinNoise2D {
   private:
   // --- Class Parameters ---
   unsigned sizeX; // The size of the terrain in the x direction
   unsigned sizeY; // The size of the terrain in the y direction
   double weightSum = 0.0; // The sum of the weights of the layers
   std::vector<PerlinLayer2D> layers; // The layers of the noise
   std::vector<vec2d> gradients; // The constant gradients used for computation
   perlin::matrix resultMatrix; // The matrix to fill

   public:
   /// @brief Noise constructor, initializes the gradients and the layers
   /// @param sizeX The size of the terrain in the x direction
   /// @param sizeY The size of the terrain in the y direction
   /// @param layerParams A vector of pairs, each pair containing the chunk size and the weight of the layer
   PerlinNoise2D(const unsigned sizeX, const unsigned sizeY, std::vector<std::pair<unsigned, double>>& layerParams) : sizeX(sizeX), sizeY(sizeY), resultMatrix(perlin::matrix(sizeX, std::vector<double>(sizeY, 0.0))) {
      gradients.resize(128);
      // initialize the gradients as random normalized 2D vectors
      for (auto& grad : gradients) {
         grad = random2DGrad();
      }

      // check if the size of the terrain is divisible by the chunk size
      for (const auto& chunkSizeWeight : layerParams) {
         if (sizeX % chunkSizeWeight.first != 0 || sizeY % chunkSizeWeight.first != 0) {
            throw std::invalid_argument("The size of the terrain must be divisible by the chunk size.");
         }
      }

      // create the layers
      for (const auto& chunkSizeWeight : layerParams) {
         auto chunkSize = chunkSizeWeight.first;
         auto weight = chunkSizeWeight.second;
         layers.push_back(PerlinLayer2D(chunkSize, sizeX / chunkSize, sizeY / chunkSize, gradients, weight));
         weightSum += weight;
      }
   }

   // --- Matrix functions ---

   perlin::matrix getResult() {
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

   /// @brief Set the layers of the noise with the already computed layers
   void setLayers(std::vector<PerlinLayer2D>& newLayers);

   /// @brief Set the layers of the noise with the parameters of the layers
   void setLayers(std::vector<std::pair<unsigned, double>>& newLayerParams);

   /// @brief Add a new layer to the noise
   void addLayer(std::pair<unsigned, double>& newLayer);

   /// @brief Remove the last added layer
   void removeLayer();

   /// @brief Remove a layer at a specific index
   void removeLayer(unsigned index);

   /// @brief Recompute a layer at a specific index with new parameters
   void recomputeLayer(unsigned index, std::pair<unsigned, double>& layerParams);

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
      double tmp = weightSum;
      updateWeightSum();
      if (tmp != weightSum) {
         throw std::logic_error("Weight sum was not measured correctly!");
      }
      return weightSum;
   }

};

}

#endif // _PERLIN_OOP