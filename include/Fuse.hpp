#ifndef FUSE_CLASS_HPP
#define FUSE_CLASS_HPP

#include "PerlinLayer.hpp"
#include <vector>

class Fuse {
   public:
   std::vector<UpdateState> noiseLayerUpdate;
   std::vector<UpdateState> baselineLayerUpdate;
   Fuse(const unsigned timeCapacity, const unsigned numNoiseLayers, const unsigned numBaselineLayers) : timeCapacity(timeCapacity) {
      noiseLayerUpdate.resize(numNoiseLayers, NONE);
      baselineLayerUpdate.resize(numBaselineLayers, NONE);
   }

   void tick();
   bool isSeedUpdateNow();
   bool isFlattenFactorUpdateNow();
   bool isLayerUpdateNow();
   void planSeedUpdate();
   void planFlattenFactorUpdate();
   void planLayerUpdate(const unsigned index, const LayerType& layerType, const UpdateState& state);
   //void resetLayerUpdateState();

   private:
   bool seedUpdate = false;
   bool flattenFactorUpdate = false;
   bool layerUpdate = false;
   unsigned timeLeft = 0;
   unsigned timeCapacity;
};
#endif