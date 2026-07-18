#pragma once
#include "core/core.h"
#include "features/data/bone.h"
#include <string>
#include <memory>

namespace wne
{
     class WNE_API Armature
     {
     public:
          Armature(const char *name);

          void addBone(const char *name, const std::vector<int> &indexes, const std::vector<float> &weights);

          uint getMaxIndex()
          {
               return maxIndex;
          }

          const char *getName()
          {
               return name.c_str();
          }

          const std::vector<std::shared_ptr<Bone>> &getBones()
          {
               return bones;
          }

     protected:
          std::string name;
          std::vector<std::shared_ptr<Bone>> bones;
          uint maxIndex = 0;
     };
};