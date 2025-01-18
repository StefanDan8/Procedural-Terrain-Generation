#ifndef TERRAIN_CLASS_HPP
#define TERRAIN_CLASS_HPP

#include "Mesh.hpp"
#include "PerlinLayer.hpp"
#include <optional>

using layerP = std::pair<unsigned, double>;

struct BasicConfigParams {
   const int seed;
   const unsigned sizeX;
   const unsigned sizeY;
   double flattenFactor = 2.0;
};

class Terrain {
   public:
   BasicConfigParams configParams;
   std::optional<Mesh> mesh;
   std::optional<std::vector<perlin::PerlinLayer>> noiseLayers;
   std::optional<std::vector<perlin::PerlinLayer>> baselineLayers;
   std::optional<perlin::matrix> noise;
   std::optional<perlin::matrix> baseline;
   const std::vector<perlin::vec2d> gradients;

   //Terrain(const std::string& configFile); TODO
   Terrain(const BasicConfigParams& basicConfigParams, const std::vector<layerP>& noiseParams, const std::vector<layerP>& baselineParams);

   // get current weight, add (weight-currentWeight)*layer to noise
   void adjustNoiseLayerWeight(const unsigned index, const double weight);
   // subtract weight*layer from noise, recompute layer, add weight*layer to noise
   void adjustNoiseLayerChunkSize(const unsigned index, const unsigned chunkSize);
   void adjustBaselineLayerWeight(const unsigned index, const double weight);
   void adjustBaselineLayerChunkSize(const unsigned index, const unsigned chunkSize);

   void initializeNoise(const std::vector<layerP>& noiseParams);
   void initializeBaseline(const std::vector<layerP>& baselineParams);

   void computeMesh();

   void Draw(Shader& shader, Camera& camera);
   //void ExportConfiguration(const std::string& filename); // to JSON
};
#endif