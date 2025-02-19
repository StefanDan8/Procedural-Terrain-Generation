#include "Fuse.hpp"

void Fuse::planFlattenFactorUpdate() {
   flattenFactorUpdate = true;
   timeLeft = timeCapacity;
}

bool Fuse::isFlattenFactorUpdateNow() {
   if (timeLeft == 0 && flattenFactorUpdate) {
      flattenFactorUpdate = false;
      return true;
   }
   return false;
}

void Fuse::tick() {
   if (seedUpdate || flattenFactorUpdate || layerUpdate) {
      timeLeft--;
   }
}

void Fuse::planSeedUpdate() {
   seedUpdate = true;
   timeLeft = timeCapacity;
}

bool Fuse::isSeedUpdateNow() {
   if (timeLeft == 0 && seedUpdate) {
      seedUpdate = false;
      return true;
   }
   return false;
}

void Fuse::planLayerUpdate(const unsigned index, const LayerType& layerType, const UpdateState& state) {
   layerUpdate = true;
   timeLeft = timeCapacity;

   if (layerType == NOISE_LAYER) {
      if (noiseLayerUpdate[index] == NONE || noiseLayerUpdate[index] == state) {
         noiseLayerUpdate[index] = state;
      } else {
         noiseLayerUpdate[index] = BOTH;
      }
   } else { // BASELINE_LAYER
      if (baselineLayerUpdate[index] == NONE || baselineLayerUpdate[index] == state) {
         baselineLayerUpdate[index] = state;

      } else {
         baselineLayerUpdate[index] = BOTH;
      }
   }
}

bool Fuse::isLayerUpdateNow() {
   if (timeLeft == 0 && layerUpdate) {
      layerUpdate = false;
      return true;
   }
   return false;
}

// void Fuse::resetLayerUpdateState() {
//    noiseLayerUpdate.resize(noiseLayerUpdate.size(), NONE);
//    baselineLayerUpdate.resize(baselineLayerUpdate.size(), NONE);
// }