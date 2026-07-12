#pragma once
#include "features/data/animationTarget.h"
#include <string>
#include <memory>

namespace wne
{
    class WNE_API Animation3d
    {
    public:
        Animation3d(const std::string &name);

        std::shared_ptr<AnimationTarget> getAnimationTarget(const std::string &targetName, bool createEmptyIfNotFound = false);

        inline const char *getName()
        {
            return name.c_str();
        }

    protected:
        std::string name;
        std::vector<std::shared_ptr<AnimationTarget>> targets;
    };
};
