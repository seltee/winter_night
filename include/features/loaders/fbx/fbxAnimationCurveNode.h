#pragma once
#include "core/core.h"
#include "fbxNode.h"
#include <memory>

namespace wne
{
    class FBXAnimationCurveNode
    {
    public:
        FBXAnimationCurveNode(FBXNode &node);

        uint64 id = 0;

        inline const std::string &getType() const
        {
            return type;
        }

        inline const Vector3 &getDefaultValue() const
        {
            return defaultValue;
        }

    private:
        std::string type = "none";
        Vector3 defaultValue{};
    };
};
