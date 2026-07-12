#include "features/data/animationTarget.h"

using namespace wne;

AnimationTarget::AnimationTarget(const std::string targetName)
{
    this->targetName = targetName;
}

float AnimationTarget::getAnimationTimeLength()
{
    if (!keys.empty())
    {
        return keys.at(keys.size() - 1).timeStamp;
    }
    return 0.0f;
}

// interpolates between 2 frames
// void getTransformByTime(float timeStamp, Entity *entity);
// picks static
// void getTransformByTimeFixedFrame(float timeStamp, Entity *entity);

void AnimationTarget::addKey(KeyTranform keyTransform)
{
    keys.push_back(keyTransform);
}