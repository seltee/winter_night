#include "features/data/armature.h"

using namespace wne;

Armature::Armature(const char *name)
{
    this->name = std::string(name);
}

void Armature::addBone(const char *name, const std::vector<int> &indexes, const std::vector<float> &weights)
{
    bones.push_back(std::make_shared<Bone>(name, indexes, weights));
}