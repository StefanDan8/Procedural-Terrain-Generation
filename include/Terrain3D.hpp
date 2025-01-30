#ifndef TERRAIN_3D_CLASS_HPP
#define TERRAIN_3D_CLASS_HPP

#include "Mesh.hpp"
#include "PerlinNoise.hpp"

class Terrain3D {
   public:
   /// @brief Constructor to initialize Terrain3D with size, seed, and flatten factor.
   /// @param sizeX Width of the terrain.
   /// @param sizeY Height of the terrain.
   /// @param seed Seed for the noise generation.
   /// @param flattenFactor Factor to flatten the terrain.
   Terrain3D(const unsigned sizeX, const unsigned sizeY, const int seed = 42, const double flattenFactor = 2.0);

   /// @brief Constructor to initialize Terrain3D with size, noise layers, baseline layers, seed, and flatten factor.
   /// @param sizeX Width of the terrain.
   /// @param sizeY Height of the terrain.
   /// @param noiseLayerParams Parameters for noise layers.
   /// @param baselineLayerParams Parameters for baseline layers.
   /// @param seed Seed for the noise generation.
   /// @param flattenFactor Factor to flatten the terrain.
   Terrain3D(const unsigned sizeX, const unsigned sizeY, std::vector<std::pair<unsigned, double>>& noiseLayerParams,
             std::vector<std::pair<unsigned, double>>& baselineLayerParams, const int seed = 42, const double flattenFactor = 2.0);

   std::optional<Mesh> mesh;
   std::vector<std::pair<unsigned, double>> noiseLayerParams;
   std::vector<std::pair<unsigned, double>> baselineLayerParams;
   perlin::PerlinNoise2D noise;
   perlin::PerlinNoise2D baseline;

   /// @brief Resize the terrain.
   /// @param sizeX New width of the terrain.
   /// @param sizeY New height of the terrain.
   void resize(const unsigned sizeX, const unsigned sizeY);

   /// @brief Change the seed for noise generation.
   /// @param seed New seed value.
   void changeSeed(const int seed);

   /// @brief Change the flatten factor of the terrain.
   /// @param flattenFactor New flatten factor value.
   void changeFlattenFactor(const double flattenFactor);

   /// @brief Adjust a layer's parameters.
   /// @param isFilterLayer Indicates if the layer is a filter layer.
   /// @param index Index of the layer to adjust.
   /// @param newChunkSize New chunk size for the layer.
   /// @param newWeight New weight for the layer.
   void adjustLayer(const bool isFilterLayer, const unsigned index, const unsigned newChunkSize, const double newWeight);

   /// @brief Recompute the terrain with new seed and flatten factor.
   /// @param seed New seed value.
   /// @param flattenFactor New flatten factor value.
   void recompute(const int seed, const double flattenFactor);

   /// @brief Get the parameters of noise layers.
   /// @return Vector of noise layer parameters.
   std::vector<std::pair<unsigned, double>> getNoiseLayersParams();

   /// @brief Get the parameters of filter layers.
   /// @return Vector of filter layer parameters.
   std::vector<std::pair<unsigned, double>> getFilterLayersParams();

   /// @brief Draw the terrain using the given shader and camera.
   /// @param shader Shader to use for drawing.
   /// @param camera Camera to use for drawing.
   void Draw(Shader& shader, Camera& camera);

   /// @brief Set the seed value.
   /// @param some_seed New seed value.
   void setSeed(const int some_seed) {
      seed = some_seed;
   }

   /// @brief Set the flatten factor value.
   /// @param some_flattenFactor New flatten factor value.
   void setFlattenFactor(const double some_flattenFactor) {
      flattenFactor = some_flattenFactor;
   }

   private:
   unsigned sizeX;
   unsigned sizeY;
   int seed;
   double flattenFactor;
};
#endif // TERRAIN_3D_CLASS_HPP