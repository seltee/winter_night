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

        const uint64 getId() const
        {
            return id;
        }

        const std::vector<KeyTime> &getKeyTimeList() const
        {
            return keyTimeList;
        }

    private:
        uint64 id = 0;
        std::vector<uint64> keyAttrReference;
        std::vector<KeyTime> keyTimeList;
    };
};
