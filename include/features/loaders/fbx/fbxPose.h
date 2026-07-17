#pragma once

#pragma once
#include "fbxNode.h"
#include <memory>

namespace wne
{
    class FBXPose
    {
    public:
        FBXPose(FBXNode &node);

        inline uint64 getId()
        {
            return id;
        }


    protected:
        uint64 id;
    };
};
