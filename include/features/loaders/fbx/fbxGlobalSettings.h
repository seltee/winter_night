#pragma once
#include "fbxNode.h"
#include "core/math.h"
#include <vector>

namespace wne
{
    class FBXGlobalSettings
    {
    public:
        FBXGlobalSettings(FBXNode &node);

    protected:
        enum class DataType
        {
            None,
            UnitScaleFactor
        };
    };
};
