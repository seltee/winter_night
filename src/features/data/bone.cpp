#include "features/data/bone.h"

using namespace wne;

Bone::Bone(const char *name,
           const std::vector<int> &indexes,
           const std::vector<float> &weights,
           const Matrix4x4 &transform)
{
    this->name = std::string(name);
    this->indexes = indexes;
    this->weights = weights;
    this->transform = transform;
}
