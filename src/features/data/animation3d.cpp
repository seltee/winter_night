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
    for (auto &target : targets)
    {
        if (target->isName(targetName))
        {
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

void Animation3d::eraseEmptyTargets()
{
    std::erase_if(targets, [&](std::shared_ptr<wne::AnimationTarget> target)
                  { 
                    uint keysCount = target->getKeysCount();
                    if (keysCount == 0)
                        return true;
                    Vector3 rot = target->getKey(0).rotation;
                    Vector3 scale = target->getKey(0).scale;

                    for (uint i = 0; i < keysCount; i++){
                        if (target->getKey(i).position.x != 0.0f || target->getKey(i).position.y != 0.0f || target->getKey(i).position.z != 0.0f ||
                            target->getKey(i).rotation.x != rot.x || target->getKey(i).rotation.y != rot.y || target->getKey(i).rotation.z != rot.z ||
                            target->getKey(i).scale.x != scale.x || target->getKey(i).scale.y != scale.y || target->getKey(i).scale.z != scale.z
                        )
                        return false;
                    }
                    return true; });
}