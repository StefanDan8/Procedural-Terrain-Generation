#include "PerlinNoise.hpp"
namespace perlin {

// ----- Noise functions -----

PerlinNoise2D::PerlinNoise2D(unsigned sizeX, unsigned sizeY, const std::vector<std::pair<unsigned, double>>& layerParams) : sizeX(sizeX), sizeY(sizeY), resultMatrix(perlin::matrix(sizeX, std::vector<double>(sizeY, 0.0))) {
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
   layers.reserve(layerParams.size());
   for (const auto& chunkSizeWeight : layerParams) {
      auto chunkSize = chunkSizeWeight.first;
      auto weight = chunkSizeWeight.second;
      layers.push_back(PerlinLayer(sizeX, sizeY, chunkSize, weight));
      weightSum += weight;
   }
}

// --- Matrix functions ---

void PerlinNoise2D::resetMatrix() {
   resultMatrix = perlin::matrix(sizeX, std::vector<double>(sizeY, 0.0));
}

void PerlinNoise2D::resizeMatrix(unsigned newSizeX, unsigned newSizeY) {
   sizeX = newSizeX;
   sizeY = newSizeY;
   // Resize outer vector to sizeX
   resultMatrix.resize(sizeX);

   // Resize each inner vector to sizeY
   for (auto& row : resultMatrix) {
      row.resize(sizeY, 0.0); // Initialize new elements with 0.0
   }
}

void PerlinNoise2D::fill() {
   for (auto& layer : layers) {
      layer.fill(gradients);
      layer.accumulate(resultMatrix, layer.getWeight());
   }
}

std::pair<double, double> PerlinNoise2D::getMinMaxVal() {
   // Find the minimum and maximum values in the matrix
   double minVal = resultMatrix[0][0];
   double maxVal = resultMatrix[0][0];
   for (const auto& row : resultMatrix) {
      for (double value : row) {
         minVal = std::min(minVal, value);
         maxVal = std::max(maxVal, value);
      }
   }

   return std::make_pair(minVal, maxVal);
}

void PerlinNoise2D::normalizeMatrix0255() {
   // Find the minimum and maximum values in the matrix
   auto minmax = getMinMaxVal();
   double minVal = minmax.first;
   double maxVal = minmax.second;

   // Normalize the matrix to [0, 255]
   for (size_t i = 0; i < sizeX; ++i) {
      for (size_t j = 0; j < sizeY; ++j) {
         resultMatrix[i][j] = static_cast<int>(255 * (resultMatrix[i][j] - minVal) / (maxVal - minVal));
      }
   }
}

void PerlinNoise2D::normalizeMatrixPM1() {
   // Find the minimum and maximum values in the matrix
   auto minmax = getMinMaxVal();
   double minVal = minmax.first;
   double maxVal = minmax.second;

   // Normalize the matrix to [-1, 1]
   for (size_t i = 0; i < sizeX; ++i) {
      for (size_t j = 0; j < sizeY; ++j) {
         resultMatrix[i][j] = 2.0f * (resultMatrix[i][j] - minVal) / (maxVal - minVal) - 1.0f;
      }
   }
}

void PerlinNoise2D::normalizeMatrixSUM(const double flatteningFactor) {
   // Normalize the matrix by dividing by the sum of the weights
   for (size_t i = 0; i < sizeX; ++i) {
      for (size_t j = 0; j < sizeY; ++j) {
         resultMatrix[i][j] /= weightSum * flatteningFactor;
      }
   }
}

void PerlinNoise2D::normalizeMatrixReLU(const double threshold) {
   // Normalize the matrix to [0, 255] and apply the ReLU function with minimal threshold to the matrix
   normalizeMatrix0255();
   matrixReLU(threshold);
}

void PerlinNoise2D::matrixReLU(const double threshold) {
   // Apply the ReLU function with minimal threshold to the matrix
   for (auto& row : resultMatrix) {
      for (auto& el : row) {
         el = std::max(el, threshold);
      }
   }
}

void PerlinNoise2D::filterMatrix(perlin::PerlinNoise2D& other) {
   // Update the own matrix with the maximum values of the own and another PerlinNoise2D object's matrix
   matrix otherMatrix = other.getResultRef();
   for (unsigned i = 0; i < sizeX; i++) {
      for (unsigned j = 0; j < sizeY; j++) {
         resultMatrix[i][j] = std::max(resultMatrix.at(i).at(j), otherMatrix.at(i).at(j));
      }
   }
}

// --- Layer functions ---

// Disable copy semantics -- copying is very inefficient
// compiler should complain if copying is done
// void PerlinNoise2D::setLayers(std::vector<PerlinLayer>& newLayers) {
//    layers = newLayers;
//    updateWeightSum();
// }

void PerlinNoise2D::setLayers(std::vector<std::pair<unsigned, double>>& newLayerParams) {
   layers.clear();
   layers.reserve(newLayerParams.size());
   weightSum = 0.0;
   // create the layers
   for (const auto& chunkSizeWeight : newLayerParams) {
      layers.push_back(PerlinLayer(sizeX, sizeY, chunkSizeWeight.first, chunkSizeWeight.second));
      weightSum += chunkSizeWeight.second;
   }
}

void PerlinNoise2D::addLayer(std::pair<unsigned, double>& newLayer) {
   auto chunkSize = newLayer.first;
   auto weight = newLayer.second;
   layers.push_back(PerlinLayer(sizeX, sizeY, chunkSize, weight));
   weightSum += weight;
}

void PerlinNoise2D::removeLayer() {
   weightSum -= layers.back().getWeight();
   layers.pop_back();
}

void PerlinNoise2D::removeLayer(unsigned index) {
   // check whether the index is valid
   if (index >= layers.size()) {
      throw std::invalid_argument("Index out of bounds");
   }
   weightSum -= layers.at(index).getWeight();
   layers.erase(layers.begin() + index);
}

void PerlinNoise2D::recomputeLayer(unsigned index, const std::pair<unsigned, double>& layerParams) {
   // first - chunk size, second - weight
   PerlinNoise2D::recomputeLayer(index, layerParams.first, layerParams.second);
}

void PerlinNoise2D::recomputeLayer(unsigned index, unsigned chunkSize, double weight) {
   if (sizeX % chunkSize != 0 || sizeY % chunkSize != 0) {
      throw std::invalid_argument("The size of the terrain must be divisible by the chunk size.");
   }
   weightSum += weight - layers.at(index).getWeight();
   layers.at(index) = PerlinLayer(sizeX, sizeY, chunkSize, weight);
}

void PerlinNoise2D::recomputeLayerChunkSize(unsigned index, unsigned chunkSize) {
   if (sizeX % chunkSize != 0 || sizeY % chunkSize != 0) {
      throw std::invalid_argument("The size of the terrain must be divisible by the chunk size.");
   }

   layers.at(index) = PerlinLayer(sizeX, sizeY, chunkSize, layers.at(index).getWeight());
}

void PerlinNoise2D::recomputeLayerWeight(unsigned index, double weight) {
   weightSum += weight - layers.at(index).getWeight();
   layers.at(index) = PerlinLayer(sizeX, sizeY, layers.at(index).getChunkSize(), weight);
}

void PerlinNoise2D::updateWeightSum() {
   weightSum = 0.0;
   for (auto& layer : layers) {
      weightSum += layer.getWeight();
   }
}

} // namespace perlin