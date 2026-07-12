#pragma once
#include "core/core.h"
#include "fbxNode.h"
#include <memory>

namespace wne
{
    class FBXAnimationLayer;

    class FBXAnimationStack
    {
    public:
        FBXAnimationStack(FBXNode &node);

        void linkLayer(FBXAnimationLayer *layer);

        const uint64 getId()
        {
            return id;
        }

    protected:
        uint64 id = 0;
        uint64 localTime = 0;
        uint64 referenceTime = 0;

        std::vector<FBXAnimationLayer *> layers;
    };
};
