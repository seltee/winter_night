#pragma once
#include "core/core.h"
#include <string>
#include <vector>

namespace wne
{
    class WNE_API Bone
    {
    public:
        Bone(const char *name, const std::vector<int> &indexes, const std::vector<float> &weights);

        const char *getName()
        {
            return name.c_str();
        }

    protected:
        std::string name;
        std::vector<int> indexes;
        std::vector<float> weights;
    };
};