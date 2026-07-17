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

        inline uint64 getId()
        {
            return id;
        }

        inline const std::string &getName() const
        {
            return name;
        }

    private:
        uint64 id = 0;
        std::string name;
    };
};
