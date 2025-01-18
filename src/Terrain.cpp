#include "Terrain.hpp"

// todo: add exceptions
void Terrain::adjustNoiseLayerWeight(const unsigned index, const double weight) {
   if (noiseLayers.has_value()) {
      if (index < noiseLayers->size()) {
         double oldWeight = (*noiseLayers)[index].getWeight();
         double weightFactor = weight - oldWeight;
         (*noiseLayers)[index].changeWeight(weight);
         if (noise.has_value()) {
            (*noiseLayers)[index].accumulate(*noise, weightFactor);
         } else {
            std::cout << "noise not initialized yet\n";
         }

      } else {
         std::cout << "index out of bounds\n";
      }
   } else {
      std::cout << "noiseLayers not initalized yet\n";
   }
}

void Terrain::adjustNoiseLayerChunkSize(const unsigned index, const unsigned chunkSize) {
   if (noiseLayers.has_value()) {
      if (index < noiseLayers->size()) {
         if (noise.has_value()) {
            double weight = (*noiseLayers)[index].getWeight();
            (*noiseLayers)[index].accumulate(*noise, -weight); // subtract old layer;
            (*noiseLayers)[index].changeChunkSize(gradients, chunkSize); // recompute layer with new chunkSize
            (*noiseLayers)[index].accumulate(*noise, weight); // add new layer back
         } else {
            std::cout << "noise not initialized yet\n";
         }
      } else {
         std::cout << "index out of bounds\n";
      }
   } else {
      std::cout << "noiseLayers not initalized yet\n";
   }
}

// todo: add exceptions
void Terrain::adjustBaselineLayerWeight(const unsigned index, const double weight) {
   if (baselineLayers.has_value()) {
      if (index < baselineLayers->size()) {
         double oldWeight = (*baselineLayers)[index].getWeight();
         double weightFactor = weight - oldWeight;
         (*baselineLayers)[index].changeWeight(weight);
         if (baseline.has_value()) {
            (*baselineLayers)[index].accumulate(*baseline, weightFactor);
         } else {
            std::cout << "baseline not initialized yet\n";
         }

      } else {
         std::cout << "index out of bounds\n";
      }
   } else {
      std::cout << "baselineLayers not initalized yet\n";
   }
}

void Terrain::adjustBaselineLayerChunkSize(const unsigned index, const unsigned chunkSize) {
   if (baselineLayers.has_value()) {
      if (index < baselineLayers->size()) {
         if (baseline.has_value()) {
            double weight = (*baselineLayers)[index].getWeight();
            (*baselineLayers)[index].accumulate(*baseline, -weight); // subtract old layer;
            (*baselineLayers)[index].changeChunkSize(gradients, chunkSize); // recompute layer with new chunkSize
            (*baselineLayers)[index].accumulate(*baseline, weight); // add new layer back
         } else {
            std::cout << "noise not initialized yet\n";
         }
      } else {
         std::cout << "index out of bounds\n";
      }
   } else {
      std::cout << "noiseLayers not initalized yet\n";
   }
}

void Terrain::initializeNoise(const std::vector<layerP>& noiseParams) {
   std::vector<perlin::PerlinLayer> layers;
   const unsigned sizeX = configParams.sizeX;
   const unsigned sizeY = configParams.sizeY;
   for (const auto& param : noiseParams) {
      layers.emplace_back(sizeX, sizeY, param.first, param.second);
   }
   noiseLayers = std::move(layers);
}

void Terrain::initializeBaseline(const std::vector<layerP>& noiseParams) {
   std::vector<perlin::PerlinLayer> layers;
   const unsigned sizeX = configParams.sizeX;
   const unsigned sizeY = configParams.sizeY;
   for (const auto& param : noiseParams) {
      layers.emplace_back(sizeX, sizeY, param.first, param.second);
   }
   baselineLayers = std::move(layers);
}

void Terrain::computeMesh() {
   // the max of noise and baseline, as well as the normalization should be done here.
   // then the mesh is constructed.
   // create a new constructor for mesh which passes both matrices.
   // as Mesh only need the y-value and iterates over the entire final result matrix anyway,
   // we can move the computation of the y-value inside the new constructor.
}

void Terrain::Draw(Shader& shader, Camera& camera) {
   if (mesh.has_value()) {
      (*mesh).Draw(shader, camera);
   }
}
