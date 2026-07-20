#pragma once
#include "core/core.h"
#include "core/math.h"
#include <string>
#include <vector>

namespace wne
{
    class WNE_API Bone
    {
    public:
        Bone(
            const char *name,
            const std::vector<int> &indexes,
            const std::vector<float> &weights,
            const Matrix4x4 &transform);

        const char *getName()
        {
            return name.c_str();
        }

        const std::vector<int> &getIndexes()
        {
            return indexes;
        }

        const std::vector<float> &getWeights()
        {
            return weights;
        }

        const Matrix4x4 &getTransform()
        {
            return transform;
        }

    protected:
        std::string name;
        std::vector<int> indexes;
        std::vector<float> weights;
        Matrix4x4 transform;
    };
};