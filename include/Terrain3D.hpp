#ifndef TERRAIN_3D_CLASS_HPP
#define TERRAIN_3D_CLASS_HPP

#include "Mesh.hpp"
#include "PerlinOOP.hpp"
#include <optional>
class Terrain {
   public:
   Terrain(const unsigned sizeX, const unsigned sizeY, const int seed = 42, const double flattenFactor = 2.0);
   Terrain(const unsigned sizeX, const unsigned sizeY, std::vector<std::pair<unsigned, double>>& noiseLayerParams,
           std::vector<std::pair<unsigned, double>>& baselineLayerParams, const int seed = 42, const double flattenFactor = 2.0);
   std::optional<Mesh> mesh;
   perlin::PerlinNoise2D noise;
   perlin::PerlinNoise2D baseline;
   void resize(const unsigned sizeX, const unsigned sizeY);
   void changeSeed(const int seed);
   void changeFlattenFactor(const double flattenFactor);
   void adjustLayer(const bool isFilterLayer, const unsigned index, const unsigned newChunkSize, const double newWeight);
   void recompute();
   void Draw(Shader& shader, Camera& camera);

   private:
   int seed;
   double flattenFactor;
};

#endif