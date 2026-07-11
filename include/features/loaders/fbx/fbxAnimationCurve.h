#pragma once
#include "core/core.h"
#include "fbxNode.h"
#include <memory>

namespace wne
{
    class FBXAnimationCurve
    {
    public:
        struct KeyTime
        {
            uint64 time;
            float value;
        };

        FBXAnimationCurve(FBXNode &node);

        uint64 id = 0;

    private:
        std::vector<uint64> keyAttrReference;
        std::vector<KeyTime> keyTimeList;
    };
};
