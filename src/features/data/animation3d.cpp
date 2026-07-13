#include "features/data/animation3d.h"
#include "features/logger/logger.h"
#include <cmath>

using namespace wne;

Animation3d::Animation3d(const std::string &name)
{
    this->name = name;
}

std::shared_ptr<AnimationTarget> Animation3d::getAnimationTarget(const std::string &targetName, bool createEmptyIfNotFound)
{
    // Logger::log << "TARGET FOR " << name.c_str() << " NAMES " << targetName << endl;
    for (auto &target : targets)
    {
        // Logger::log << target->getTargetName() << " " << target->getKeysCount() << endl;
    }
    for (auto &target : targets)
    {
        if (target->isName(targetName))
        {
            // Logger::log << "Target found " << targetName << " " << target->getKeysCount() << endl;
            return target;
        }
    }

    if (createEmptyIfNotFound)
    {
        auto newAnimationTarget = std::make_shared<AnimationTarget>(targetName);
        targets.push_back(newAnimationTarget);
        return newAnimationTarget;
    }

    return nullptr;
}

float Animation3d::getAnimationLength()
{
    float maxLength = 0.0f;

    for (auto &target : targets)
    {
        maxLength = std::max(maxLength, target->getAnimationTimeLength());
    }

    return maxLength;
}

void Animation3d::eraseTargetsWithKeysLessThan(uint amount)
{
    std::erase_if(targets, [&](std::shared_ptr<wne::AnimationTarget> target)
                  { return target->getKeysCount() < amount; });
}