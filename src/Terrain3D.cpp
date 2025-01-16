#include "Terrain3D.hpp"

Terrain::Terrain(const unsigned sizeX, const unsigned sizeY, const int seed = 42, const double flattenFactor = 2.0)
   : seed(seed),
     flattenFactor(flattenFactor),
     noise(sizeX, sizeY, std::vector<std::pair<unsigned, double>>{{720, 30}, {360, 250}, {180, 50}, {90, 50}, {45, 20}, {12, 5}, {8, 2}, {3, 1}}),
     baseline(sizeX, sizeY, std::vector<std::pair<unsigned, double>>{{180, 2}, {120, 2}, {60, 2}, {30, 1}}) {
   noise.fill();
   baseline.fill();
   noise.filterMatrix(baseline);
   noise.normalizeMatrixSUM(flattenFactor);
   mesh.emplace(noise.getResult());
}

Terrain::Terrain(const unsigned sizeX, const unsigned sizeY, std::vector<std::pair<unsigned, double>>& noiseLayerParams,
                 std::vector<std::pair<unsigned, double>>& baselineLayerParams, const int seed = 42, const double flattenFactor = 2.0)
   : seed(seed), flattenFactor(flattenFactor), noise(sizeX, sizeY, noiseLayerParams), baseline(sizeX, sizeY, baselineLayerParams) {
   noise.fill();
   baseline.fill();
   noise.filterMatrix(baseline);
   noise.normalizeMatrixSUM(flattenFactor);
   mesh.emplace(noise.getResult());
}

void Terrain::adjustLayer(const bool isFilterLayer, const unsigned index, const unsigned newChunkSize, const double newWeight) {
   if (isFilterLayer) {
      baseline.recomputeLayer(index, {newChunkSize, newWeight});
   } else {
      noise.recomputeLayer(index, {newChunkSize, newWeight});
   }
}

void Terrain::recompute() {
   noise.resetMatrix();
   noise.fill();
   baseline.resetMatrix();
   baseline.fill();
   noise.filterMatrix(baseline);
   noise.normalizeMatrixSUM(flattenFactor);
   mesh.emplace(noise.getResult());
}

void Terrain::Draw(Shader& shader, Camera& camera) {
   mesh->Draw(shader, camera);
}