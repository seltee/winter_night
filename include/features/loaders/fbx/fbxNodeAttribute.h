#pragma once
#include "core/core.h"
#include "fbxNode.h"
#include <memory>

namespace wne
{
    class FBXNodeAttribute
    {
    public:
        FBXNodeAttribute(FBXNode &node);

        uint64 id = 0;

        inline const std::string &getName() const
        {
            return name;
        }

    private:
        std::string name;
    };
};
