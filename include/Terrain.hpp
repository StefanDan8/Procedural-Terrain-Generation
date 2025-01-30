#ifndef TERRAIN_CLASS_HPP
#define TERRAIN_CLASS_HPP

#include "Mesh.hpp"
#include "PerlinLayer.hpp"

using layerP = std::pair<unsigned, double>;

struct BasicConfigParams {
   int seed;
   const unsigned sizeX;
   const unsigned sizeY;
   double flattenFactor = 2.0;
};

class Terrain {
   private:
   BasicConfigParams configParams;
   std::optional<Mesh> mesh;
   std::optional<std::vector<perlin::PerlinLayer>> noiseLayers;
   std::optional<std::vector<perlin::PerlinLayer>> baselineLayers;
   std::optional<perlin::matrix> noise;
   std::optional<perlin::matrix> baseline;
   std::vector<perlin::vec2d> gradients;
   std::vector<layerP> noiseParams;
   std::vector<layerP> baselineParams;

   public:

   // //Terrain(const std::string& configFile); TODO
   // Terrain(const BasicConfigParams& basicConfigParams, const std::vector<layerP>& noiseParams, const std::vector<layerP>& baselineParams);

   // // get current weight, add (weight-currentWeight)*layer to noise
   // void adjustNoiseLayerWeight(const unsigned index, const double weight);
   // // subtract weight*layer from noise, recompute layer, add weight*layer to noise
   // void adjustNoiseLayerChunkSize(const unsigned index, const unsigned chunkSize);
   // void adjustBaselineLayerWeight(const unsigned index, const double weight);
   // void adjustBaselineLayerChunkSize(const unsigned index, const unsigned chunkSize);

   // void adjustNoiseBoth(const unsigned index, const layerP pair);
   // void adjustBaselineBoth(const unsigned index, const layerP pair);

   // void initializeNoise(const std::vector<layerP>& noiseParams);
   // void initializeBaseline(const std::vector<layerP>& baselineParams);

   // void computeMesh(const double flattenFactor);

   // void createFromSeed(const int newSeed);

   // void Draw(Shader& shader, Camera& camera);
   // //void ExportConfiguration(const std::string& filename); // to JSON

   // void recomputeLayers(std::vector<UpdateState>& noiseLayerUpdate, std::vector<UpdateState>& baselineLayerUpdate);

   /// @brief Constructor to initialize Terrain with basic configuration parameters, noise parameters, and baseline parameters.
   /// @param basicConfigParams Basic configuration parameters.
   /// @param noiseParams Parameters for noise layers.
   /// @param baselineParams Parameters for baseline layers.
   Terrain(const BasicConfigParams& basicConfigParams, const std::vector<layerP>& noiseParams, const std::vector<layerP>& baselineParams);

   /// @brief Adjust the weight of a noise layer.
   /// @param index Index of the noise layer.
   /// @param weight New weight for the noise layer.
   void adjustNoiseLayerWeight(const unsigned index, const double weight);

   /// @brief Adjust the chunk size of a noise layer.
   /// @param index Index of the noise layer.
   /// @param chunkSize New chunk size for the noise layer.
   void adjustNoiseLayerChunkSize(const unsigned index, const unsigned chunkSize);

   /// @brief Adjust the weight of a baseline layer.
   /// @param index Index of the baseline layer.
   /// @param weight New weight for the baseline layer.
   void adjustBaselineLayerWeight(const unsigned index, const double weight);

   /// @brief Adjust the chunk size of a baseline layer.
   /// @param index Index of the baseline layer.
   /// @param chunkSize New chunk size for the baseline layer.
   void adjustBaselineLayerChunkSize(const unsigned index, const unsigned chunkSize);

   /// @brief Adjust both weight and chunk size of a noise layer.
   /// @param index Index of the noise layer.
   /// @param pair Pair containing new chunk size and weight for the noise layer.
   void adjustNoiseBoth(const unsigned index, const layerP pair);

   /// @brief Adjust both weight and chunk size of a baseline layer.
   /// @param index Index of the baseline layer.
   /// @param pair Pair containing new chunk size and weight for the baseline layer.
   void adjustBaselineBoth(const unsigned index, const layerP pair);

   /// @brief Initialize noise layers with given parameters.
   /// @param noiseParams Parameters for noise layers.
   void initializeNoise(const std::vector<layerP>& noiseParams);

   /// @brief Initialize baseline layers with given parameters.
   /// @param baselineParams Parameters for baseline layers.
   void initializeBaseline(const std::vector<layerP>& baselineParams);

   /// @brief Compute the mesh with a given flatten factor.
   /// @param flattenFactor Factor to flatten the terrain.
   void computeMesh(const double flattenFactor);

   /// @brief Create terrain from a given seed.
   /// @param newSeed Seed for the noise generation.
   void createFromSeed(const int newSeed);

   /// @brief Draw the terrain using the given shader and camera.
   /// @param shader Shader to use for drawing.
   /// @param camera Camera to use for drawing.
   void Draw(Shader& shader, Camera& camera);

   /// @brief Recompute layers with given update states.
   /// @param noiseLayerUpdate Update states for noise layers.
   /// @param baselineLayerUpdate Update states for baseline layers.
   void recomputeLayers(std::vector<UpdateState>& noiseLayerUpdate, std::vector<UpdateState>& baselineLayerUpdate);

   // --- Getter functions ---

   std::vector<layerP>& getNoiseParams() {
      return noiseParams;
   }

   std::vector<layerP>& getBaselineParams() {
      return baselineParams;
   }

   Mesh& getMesh() {
      return mesh.value();
   }

}; // class Terrain

#endif // TERRAIN_CLASS_HPP