#include "features/data/armature.h"
#include <cmath>

using namespace wne;

Armature::Armature(const char *name)
{
    this->name = std::string(name);
}

void Armature::addBone(const char *name, const std::vector<int> &indexes, const std::vector<float> &weights)
{
    bones.push_back(std::make_shared<Bone>(name, indexes, weights));
    for (uint i = 0; i < indexes.size(); i++)
        maxIndex = std::max(maxIndex, (uint)indexes[i]);
}