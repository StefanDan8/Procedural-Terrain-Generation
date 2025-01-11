#include "PerlinOOP.hpp"
namespace perlin {

UniformUnitGenerator unifGlbl = UniformUnitGenerator(631); // Global random number generator for Unif[0,1]
// ----- General functions -----

int simpleHash(int i, int j, int N) {
   return (42043 * i + 15299 * j) % N;
}

vec2d random2DGrad() {
   return random2DGrad(unifGlbl);
}

vec2d random2DGrad(UniformUnitGenerator& unif) {
   auto theta = unif.get() * 2 * M_PI;
   return vec2d{cos(theta), sin(theta)};
}

vec3d random3DGrad() {
   return random3DGrad(unifGlbl);
}

vec3d random3DGrad(UniformUnitGenerator& unif) {
   auto cosphi = unif.get() * 2 - 1;
   auto theta = unif.get() * 2 * M_PI;
   auto h = sqrt(1 - cosphi * cosphi);
   return vec3d{h * cos(theta), h * sin(theta), cosphi};
}

double dot(vec2d& x, vec2d& y) {
   return x[0] * y[0] + x[1] * y[1];
}

double dot(vec3d& x, vec3d& y) {
   return x[0] * y[0] + x[1] * y[1] + x[2] * y[2];
}

double fade(const double t) {
   return t * t * t * (t * (t * 6 - 15) + 10);
}

double lerp(const double a, const double b, const double t) {
   return a + t * (b - a);
}



// ----- Layer functions -----

double PerlinLayer2D::computeWithIndices(const unsigned x, const unsigned y, const int valBL, const int valBR, const int valTL, const int valTR) {
   // Compute the position of the point within the square
   double dx = (x % chunkSize + 1) / (double) chunkSize;
   double dy = (y % chunkSize + 1) / (double) chunkSize;

   // Find the vectors pointing to the point from the 4 corners of the square
   vec2d BL = {dx, dy};
   vec2d BR = {dx - 1.0, dy};
   vec2d TL = {dx, dy - 1.0};
   vec2d TR = {dx - 1.0, dy - 1.0};

   // Compute the corresponding dot products
   double dotBL = dot(gradients.at(valBL), BL);
   double dotBR = dot(gradients.at(valBR), BR);
   double dotTL = dot(gradients.at(valTL), TL);
   double dotTR = dot(gradients.at(valTR), TR);

   // Compute fade curves for x and y
   double u = fade(dx);
   double v = fade(dy);

   // Interpolate the 4 results
   return (lerp(lerp(dotBL, dotBR, u), lerp(dotTL, dotTR, u), v)) * weight;
}
double PerlinLayer2D::compute(const unsigned x, const unsigned y, const unsigned chunkX, const unsigned chunkY) {
   // Compute the position of the point within the square
   double dx = (x % chunkSize + 1) / (double) chunkSize;
   double dy = (y % chunkSize + 1) / (double) chunkSize;

   // Find the vectors pointing to the point from the 4 corners of the square
   vec2d BL = {dx, dy};
   vec2d BR = {dx - 1.0, dy};
   vec2d TL = {dx, dy - 1.0};
   vec2d TR = {dx - 1.0, dy - 1.0};

   // Select a value for each of the 4 corners of the square from the permutation table
   auto size = gradients.size();
   int valBL = simpleHash(chunkX, chunkY, size);
   int valBR = simpleHash(chunkX + 1, chunkY, size);
   int valTL = simpleHash(chunkX, chunkY + 1, size);
   int valTR = simpleHash(chunkX + 1, chunkY + 1, size);

   // Compute the corresponding dot products
   double dotBL = dot(gradients.at(valBL), BL);
   double dotBR = dot(gradients.at(valBR), BR);
   double dotTL = dot(gradients.at(valTL), TL);
   double dotTR = dot(gradients.at(valTR), TR);

   // Compute fade curves for x and y
   double u = fade(dx);
   double v = fade(dy);

   // Interpolate the 4 results
   return (lerp(lerp(dotBL, dotBR, u), lerp(dotTL, dotTR, u), v)) * weight;
}

void PerlinLayer2D::fillChunk(matrix& result, const unsigned chunkX, const unsigned chunkY) {
   const unsigned offsetX = chunkSize * chunkX;
   const unsigned offsetY = chunkSize * chunkY;

   // Select a value for each of the 4 corners of the square from the permutation table
   auto size = gradients.size();
   int valBL = simpleHash(chunkX, chunkY, size);
   int valBR = simpleHash(chunkX + 1, chunkY, size);
   int valTL = simpleHash(chunkX, chunkY + 1, size);
   int valTR = simpleHash(chunkX + 1, chunkY + 1, size);

   for (unsigned i = offsetX; i < offsetX + chunkSize; i++) {
      for (unsigned j = offsetY; j < offsetY + chunkSize; j++) {
         //result[i][j] += compute(i, j, chunkX, chunkY);
         result[i][j] += computeWithIndices(i, j, valBL, valBR, valTL, valTR);
      }
   }
}

void PerlinLayer2D::fillMatrix(matrix& result) {
   for (unsigned chunkX = 0; chunkX < numChunksX; chunkX++) {
      for (unsigned chunkY = 0; chunkY < numChunksY; chunkY++) {
         fillChunk(result, chunkX, chunkY);
      }
   }
}

// ----- Noise functions -----

// -- Matrix functions --

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
      layer.fillMatrix(resultMatrix);
   }
}

std::pair<double, double> PerlinNoise2D::getMinMaxVal(){
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

void PerlinNoise2D::normalizeMatrix0255(){
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

void PerlinNoise2D::normalizeMatrixPM1(){
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

void PerlinNoise2D::normalizeMatrixSUM(const double flatteningFactor){
   // Normalize the matrix by dividing by the sum of the weights
   for (size_t i = 0; i < sizeX; ++i) {
      for (size_t j = 0; j < sizeY; ++j) {
         resultMatrix[i][j] /= weightSum * flatteningFactor;
      }
   }
}

void PerlinNoise2D::normalizeMatrixReLU(const double threshold){
   // Normalize the matrix to [0, 255] and apply the ReLU function with minimal threshold to the matrix
   normalizeMatrix0255();
   matrixReLU(threshold);
}

void PerlinNoise2D::matrixReLU(const double threshold){
   // Apply the ReLU function with minimal threshold to the matrix
   for (auto& row : resultMatrix) {
      for (auto& el : row) {
         el = std::max(el, threshold);
      }
   }
}

void PerlinNoise2D::filterMatrix(perlin::PerlinNoise2D& other){
   // Update the own matrix with the maximum values of the own and another PerlinNoise2D object's matrix
   auto otherMatrix = other.getResult();
   for (unsigned i = 0; i < sizeX; i++) {
      for (unsigned j = 0; j < sizeY; j++) {
         resultMatrix[i][j] = std::max(resultMatrix[i][j], otherMatrix[i][j]);
      }
   }
}

// --- Layer functions ---

void PerlinNoise2D::setLayers(std::vector<PerlinLayer2D>& newLayers) {
   layers = newLayers;
   updateWeightSum();
}

void PerlinNoise2D::setLayers(std::vector<std::pair<unsigned, double>>& newLayerParams) {
   layers = {};
   weightSum = 0.0;
   // create the layers
   for (const auto& chunkSizeWeight : newLayerParams) {
      auto chunkSize = chunkSizeWeight.first;
      auto weight = chunkSizeWeight.second;
      layers.push_back(PerlinLayer2D(chunkSize, sizeX / chunkSize, sizeY / chunkSize, gradients, weight));
      weightSum += weight;
   }
}

void PerlinNoise2D::addLayer(std::pair<unsigned, double>& newLayer) {
   auto chunkSize = newLayer.first;
   auto weight = newLayer.second;
   layers.push_back(PerlinLayer2D(chunkSize, sizeX / chunkSize, sizeY / chunkSize, gradients, weight));
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

void PerlinNoise2D::recomputeLayer(unsigned index, std::pair<unsigned, double>& layerParams) {
   if (sizeX % layerParams.first != 0 || sizeY % layerParams.first != 0) {
      throw std::invalid_argument("The size of the terrain must be divisible by the chunk size.");
   }
   weightSum += layerParams.second - layers.at(index).getWeight();
   layers.at(index) = PerlinLayer2D(layerParams.first, sizeX / layerParams.first, sizeY / layerParams.first, gradients, layerParams.second);
}

void PerlinNoise2D::recomputeLayer(unsigned index, unsigned chunkSize, double weight) {
   if (sizeX % chunkSize != 0 || sizeY % chunkSize != 0) {
      throw std::invalid_argument("The size of the terrain must be divisible by the chunk size.");
   }
   weightSum += weight - layers.at(index).getWeight();
   layers.at(index) = PerlinLayer2D(chunkSize, sizeX / chunkSize, sizeY / chunkSize, gradients, weight);
}

void PerlinNoise2D::recomputeLayerChunkSize(unsigned index, unsigned chunkSize) {
   if (sizeX % chunkSize != 0 || sizeY % chunkSize != 0) {
      throw std::invalid_argument("The size of the terrain must be divisible by the chunk size.");
   }

   layers.at(index) = PerlinLayer2D(chunkSize, sizeX / chunkSize, sizeY / chunkSize, gradients, layers.at(index).getWeight());
}

void PerlinNoise2D::recomputeLayerWeight(unsigned index, double weight) {
   weightSum += weight - layers.at(index).getWeight();
   layers.at(index) = PerlinLayer2D(layers.at(index).getChunkSize(), sizeX / layers.at(index).getChunkSize(), sizeY / layers.at(index).getChunkSize(), gradients, weight);
}

void PerlinNoise2D::updateWeightSum() {
   weightSum = 0.0;
   for (auto& layer : layers) {
      weightSum += layer.getWeight();
   }
}

} // namespace perlin