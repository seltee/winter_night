#pragma once
#include "core/core.h"
#include "fbxNode.h"
#include <memory>

namespace wne
{
    class FBXAnimationLayer
    {
    public:
        FBXAnimationLayer(FBXNode &node);

        uint64 id = 0;
        std::string name;
    };
};
