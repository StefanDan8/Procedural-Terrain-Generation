#include "Terrain3D.hpp"

Terrain3D::Terrain3D(const unsigned sizeX, const unsigned sizeY, const int seed, const double flattenFactor)
   : sizeX(sizeX),
     sizeY(sizeY),
     seed(seed),
     flattenFactor(flattenFactor),
     noiseLayerParams(std::vector<std::pair<unsigned, double>>{{720, 30}, {360, 250}, {180, 50}, {90, 50}, {45, 20}, {12, 5}, {8, 2}, {3, 1}}),
     baselineLayerParams(std::vector<std::pair<unsigned, double>>{{180, 2}, {120, 2}, {60, 2}, {30, 1}}),
     noise(sizeX, sizeY, noiseLayerParams),
     baseline(sizeX, sizeY, baselineLayerParams) {
   noise.fill();
   baseline.fill();
   noise.filterMatrix(baseline);
   noise.normalizeMatrixSUM(flattenFactor);
   mesh.emplace(noise.getResult());
}

Terrain3D::Terrain3D(const unsigned sizeX, const unsigned sizeY, std::vector<std::pair<unsigned, double>>& noiseLayerParams,
                     std::vector<std::pair<unsigned, double>>& baselineLayerParams, const int seed, const double flattenFactor)
   : sizeX(sizeX), sizeY(sizeY),
     seed(seed), flattenFactor(flattenFactor),
     noiseLayerParams(noiseLayerParams), baselineLayerParams(baselineLayerParams),
     noise(sizeX, sizeY, noiseLayerParams), baseline(sizeX, sizeY, baselineLayerParams) {
   noise.fill();
   baseline.fill();
   noise.filterMatrix(baseline);
   noise.normalizeMatrixSUM(flattenFactor);
   mesh.emplace(noise.getResult());
}

void Terrain3D::adjustLayer(const bool isFilterLayer, const unsigned index, const unsigned newChunkSize, const double newWeight) {
   if (isFilterLayer) {
      baselineLayerParams[index] = {newChunkSize, newWeight};
      baseline.recomputeLayer(index, baselineLayerParams[index]);
   } else {
      noiseLayerParams[index] = {newChunkSize, newWeight};
      noise.recomputeLayer(index, noiseLayerParams[index]);
   }
}

void Terrain3D::recompute(const int new_seed, const double new_flattenFactor) {
   seed = new_seed;
   flattenFactor = new_flattenFactor;
   perlin::AppConfig::getInstance().setGenerator(seed);
   std::cout << noiseLayerParams.size() << std::endl;
   std::cout << baselineLayerParams.size() << std::endl;
   noise = perlin::PerlinNoise2D(sizeX, sizeY, noiseLayerParams);
   std::cout << noise.getWeightSum() << std::endl;
   baseline = perlin::PerlinNoise2D(sizeX, sizeY, baselineLayerParams);
   std::cout << baseline.getWeightSum() << std::endl;
   noise.fill();
   std::cout << "Filled 1\n";
   baseline.fill();
   std::cout << "Filled 2\n";
   noise.filterMatrix(baseline);

   noise.normalizeMatrixSUM(flattenFactor);
   mesh.emplace(noise.getResult());
}

void Terrain3D::Draw(Shader& shader, Camera& camera) {
   mesh->Draw(shader, camera);
}