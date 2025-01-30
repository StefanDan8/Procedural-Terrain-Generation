#ifndef TERRAIN_3D_CLASS_HPP
#define TERRAIN_3D_CLASS_HPP

#include "Mesh.hpp"
#include "PerlinNoise.hpp"
#include <optional>
class Terrain3D {
   public:
   Terrain3D(const unsigned sizeX, const unsigned sizeY, const int seed = 42, const double flattenFactor = 2.0);
   Terrain3D(const unsigned sizeX, const unsigned sizeY, std::vector<std::pair<unsigned, double>>& noiseLayerParams,
             std::vector<std::pair<unsigned, double>>& baselineLayerParams, const int seed = 42, const double flattenFactor = 2.0);
   std::optional<Mesh> mesh;
   std::vector<std::pair<unsigned, double>> noiseLayerParams;
   std::vector<std::pair<unsigned, double>> baselineLayerParams;
   perlin::PerlinNoise2D noise;
   perlin::PerlinNoise2D baseline;
   void resize(const unsigned sizeX, const unsigned sizeY);
   void changeSeed(const int seed);
   void changeFlattenFactor(const double flattenFactor);
   void adjustLayer(const bool isFilterLayer, const unsigned index, const unsigned newChunkSize, const double newWeight);
   void recompute(const int seed, const double flattenFactor);
   std::vector<std::pair<unsigned, double>> getNoiseLayersParams();
   std::vector<std::pair<unsigned, double>> getFilterLayersParams();
   void Draw(Shader& shader, Camera& camera);

   void setSeed(const int some_seed) {
      seed = some_seed;
   }
   void setFlattenFactor(const double some_flattenFactor) {
      flattenFactor = some_flattenFactor;
   }

   private:
   unsigned sizeX;
   unsigned sizeY;
   int seed;
   double flattenFactor;
};

#endif