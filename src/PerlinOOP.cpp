#include "PerlinOOP.hpp"
namespace perlin {

// --- Layer functions ---

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
   for (unsigned i = offsetX; i < offsetX + chunkSize; i++) {
      for (unsigned j = offsetY; j < offsetY + chunkSize; j++) {
         result[i][j] += compute(i, j, chunkX, chunkY);
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

// --- Noise functions ---

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

   // Normalize the result
   for (auto& row : resultMatrix) {
      for (auto& val : row) {
         val /= weightSum;
      }
   }
}

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

void PerlinNoise2D::addLayer(std::pair<unsigned, double>& newLayer){
   auto chunkSize = newLayer.first;
   auto weight = newLayer.second;
   layers.push_back(PerlinLayer2D(chunkSize, sizeX / chunkSize, sizeY / chunkSize, gradients, weight));
   weightSum += weight;
}

void PerlinNoise2D::removeLayer(){
   weightSum -= layers.back().getWeight();
   layers.pop_back();
}

void PerlinNoise2D::removeLayer(unsigned index){
   // check whether the index is valid
   if (index >= layers.size()) {
      throw std::invalid_argument("Index out of bounds");
   }
   weightSum -= layers.at(index).getWeight();
   layers.erase(layers.begin() + index);
}

void PerlinNoise2D::recomputeLayer(unsigned index, std::pair<unsigned, double>& layerParams){
   if (sizeX % layerParams.first != 0 || sizeY % layerParams.first != 0) {
            throw std::invalid_argument("The size of the terrain must be divisible by the chunk size.");
   }
   weightSum += layerParams.second - layers.at(index).getWeight();
   layers.at(index) = PerlinLayer2D(layerParams.first, sizeX / layerParams.first, sizeY / layerParams.first, gradients, layerParams.second);
}

void PerlinNoise2D::recomputeLayer(unsigned index, unsigned chunkSize, double weight){
   if (sizeX % chunkSize != 0 || sizeY % chunkSize != 0) {
            throw std::invalid_argument("The size of the terrain must be divisible by the chunk size.");
   }
   weightSum += weight - layers.at(index).getWeight();
   layers.at(index) = PerlinLayer2D(chunkSize, sizeX / chunkSize, sizeY / chunkSize, gradients, weight);
}

void PerlinNoise2D::recomputeLayerChunkSize(unsigned index, unsigned chunkSize){
   if (sizeX % chunkSize != 0 || sizeY % chunkSize != 0) {
            throw std::invalid_argument("The size of the terrain must be divisible by the chunk size.");
   }

   layers.at(index) = PerlinLayer2D(chunkSize, sizeX / chunkSize, sizeY / chunkSize, gradients, layers.at(index).getWeight());
}


void PerlinNoise2D::recomputeLayerWeight(unsigned index, double weight){
   weightSum += weight - layers.at(index).getWeight();
   layers.at(index) = PerlinLayer2D(layers.at(index).getChunkSize(), sizeX / layers.at(index).getChunkSize(), sizeY / layers.at(index).getChunkSize(), gradients, weight);
}

void PerlinNoise2D::updateWeightSum(){
   weightSum = 0.0;
   for (auto& layer : layers) {
      weightSum += layer.getWeight();
   }
}


} // namespace perlin