#pragma once
#include "core/core.h"
#include "fbxNode.h"
#include <memory>

namespace wne
{
    class FBXAnimationStack
    {
    public:
        FBXAnimationStack(FBXNode &node);

        uint64 id = 0;
        uint64 localTime = 0;
        uint64 referenceTime = 0;
    };
};
