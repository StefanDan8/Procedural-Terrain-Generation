#include "Terrain.hpp"

Terrain::Terrain(const BasicConfigParams& basicConfigParams, const std::vector<layerP>& noiseParams, const std::vector<layerP>& baselineParams)
   : configParams(basicConfigParams),
     noiseParams(noiseParams),
     baselineParams(baselineParams) {
   createFromSeed(configParams.seed);
}

void Terrain::createFromSeed(const int newSeed) {
   configParams.seed = newSeed;
   // Create gradients
   perlin::AppConfig::getInstance().setGenerator(configParams.seed);
   gradients.resize(128);
   for (auto& vec : gradients) {
      vec = perlin::random2DGrad();
   }
   // Create Layers and noise matrices
   initializeNoise(noiseParams);
   initializeBaseline(baselineParams);
   // Create Mesh
   computeMesh(configParams.flattenFactor);
   std::cout << "Terrain complete\n";
}
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

void Terrain::adjustNoiseBoth(const unsigned index, const layerP pair) {
   if (noiseLayers.has_value()) {
      if (index < noiseLayers->size()) {
         if (noise.has_value()) {
            double weight = (*noiseLayers)[index].getWeight();
            (*noiseLayers)[index].accumulate(*noise, -weight); // subtract old layer;
            (*noiseLayers)[index].changeChunkSize(gradients, pair.first); // recompute layer with new chunkSize
            (*noiseLayers)[index].changeWeight(pair.second);
            (*noiseLayers)[index].accumulate(*noise, pair.second); // add new layer back
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

void Terrain::adjustBaselineBoth(const unsigned index, const layerP pair) {
   if (baselineLayers.has_value()) {
      if (index < baselineLayers->size()) {
         if (baseline.has_value()) {
            double weight = (*baselineLayers)[index].getWeight();
            (*baselineLayers)[index].accumulate(*baseline, -weight); // subtract old layer;
            (*baselineLayers)[index].changeChunkSize(gradients, pair.first); // recompute layer with new chunkSize
            (*baselineLayers)[index].changeWeight(pair.second);
            (*baselineLayers)[index].accumulate(*baseline, pair.second); // add new layer back
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
   noise.emplace(perlin::matrix(sizeX, std::vector<double>(sizeY, 0.0)));
   int i = 0;
   for (const auto& param : noiseParams) {
      layers.emplace_back(sizeX, sizeY, param.first, param.second);
      layers[i].fill(gradients);
      layers[i++].accumulate(*noise, param.second); //weight
   }
   noiseLayers = std::move(layers);
}

void Terrain::initializeBaseline(const std::vector<layerP>& noiseParams) {
   std::vector<perlin::PerlinLayer> layers;
   const unsigned sizeX = configParams.sizeX;
   const unsigned sizeY = configParams.sizeY;
   baseline.emplace(perlin::matrix(sizeX, std::vector<double>(sizeY, 0.0)));
   int i = 0;
   for (const auto& param : noiseParams) {
      layers.emplace_back(sizeX, sizeY, param.first, param.second);
      layers[i].fill(gradients);
      layers[i++].accumulate(*baseline, param.second); //weight
   }
   baselineLayers = std::move(layers);
}

void Terrain::computeMesh(const double flattenFactor) {
   // the max of noise and baseline, as well as the normalization should be done here.
   // then the mesh is constructed.
   // create a new constructor for mesh which passes both matrices.
   // as Mesh only need the y-value and iterates over the entire final result matrix anyway,
   // we can move the computation of the y-value inside the new constructor.
   // check for optionals
   configParams.flattenFactor = flattenFactor;
   double normalizingFactor = 0.0;
   for (auto& layer : *noiseLayers) {
      normalizingFactor += layer.getWeight();
   }
   normalizingFactor *= flattenFactor;
   auto& noiseMatrix = *noise;
   auto& baselineMatrix = *baseline;
   unsigned numX = noiseMatrix.size() - 1;
   unsigned numY = noiseMatrix[0].size() - 1;
   unsigned numVertices = (numX + 1) * (numY + 1);
   unsigned numFaces = numX * numY;
   std::vector<Vertex> _vertices(numVertices);
   std::vector<GLuint> _indices(numFaces * 6);
   float invNumX = 1.0f / numX;
   float invNumY = 1.0f / numY;

   for (unsigned j = 0; j <= numY; ++j) {
      for (unsigned i = 0; i <= numX; ++i) {
         const double yValue = std::max(baselineMatrix[i][j], noiseMatrix[i][j]) / normalizingFactor;

         glm::vec3 coordinates = glm::vec3(i * invNumX - 0.5f, yValue, j * invNumY - 0.5f);
         glm::vec3 normal = glm::vec3(0.0f);
         glm::vec2 texCoordinates = glm::vec2(i * invNumX, j * invNumY);

         _vertices[j * (numX + 1) + i] = Vertex{coordinates, normal, glm::vec3(0.3f, 0.70f, 0.44f), texCoordinates};
      }
   }
   for (unsigned j = 0, k = 0; j < numY; ++j) {
      for (unsigned i = 0; i < numX; ++i) {
         _indices[k] = j * (numX + 1) + i;
         _indices[k + 1] = j * (numX + 1) + i + 1;
         _indices[k + 2] = (j + 1) * (numX + 1) + i + 1;
         _indices[k + 3] = (j + 1) * (numX + 1) + i + 1;
         _indices[k + 4] = (j + 1) * (numX + 1) + i;
         _indices[k + 5] = j * (numX + 1) + i;
         k += 6;
      }
   }
   mesh.emplace(_vertices, _indices);
}

void Terrain::Draw(Shader& shader, Camera& camera) {
   if (mesh.has_value()) {
      (*mesh).Draw(shader, camera);
   }
}

void Terrain::recomputeLayers(std::vector<UpdateState>& noiseLayerUpdate, std::vector<UpdateState>& baselineLayerUpdate) {
   for (unsigned i = 0; i < noiseLayerUpdate.size(); ++i) {
      switch (noiseLayerUpdate[i]) {
         case WEIGHT:
            adjustNoiseLayerWeight(i, noiseParams[i].second);
            break;
         case CHUNK_SIZE:
            adjustNoiseLayerChunkSize(i, noiseParams[i].first);
            break;
         case BOTH:
            adjustNoiseBoth(i, noiseParams[i]);
         default:
            break;
      }
      noiseLayerUpdate[i] = NONE;
   }
   for (unsigned i = 0; i < baselineLayerUpdate.size(); ++i) {
      switch (baselineLayerUpdate[i]) {
         case WEIGHT:
            adjustBaselineLayerWeight(i, baselineParams[i].second);
            break;
         case CHUNK_SIZE:
            adjustBaselineLayerChunkSize(i, baselineParams[i].first);
            break;
         case BOTH:
            adjustBaselineBoth(i, baselineParams[i]);
         default:
            break;
      }
      baselineLayerUpdate[i] = NONE;
   }
   computeMesh(configParams.flattenFactor);
}
