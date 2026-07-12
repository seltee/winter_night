#include "features/data/animation3d.h"

using namespace wne;

Animation3d::Animation3d(const std::string &name)
{
    this->name = name;
}

std::shared_ptr<AnimationTarget> Animation3d::getAnimationTarget(const std::string &targetName, bool createEmptyIfNotFound)
{
    for (auto &target : targets)
    {
        if (target->isName(targetName))
            return target;
    }

    if (createEmptyIfNotFound)
    {
        auto newAnimationTarget = std::make_shared<AnimationTarget>(targetName);
        targets.push_back(newAnimationTarget);
        return newAnimationTarget;
    }

    return nullptr;
}
