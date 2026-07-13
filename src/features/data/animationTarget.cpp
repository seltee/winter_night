#include "features/data/animationTarget.h"
#include "features/logger/logger.h"

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
Matrix4x4 AnimationTarget::getTransformByTime(float timeStamp)
{
    int size = keys.size();
    for (int i = 1; i < size; i++)
    {
        if (timeStamp < keys[i].timeStamp)
        {
            float time = (timeStamp - keys[i - 1].timeStamp) / (keys[i].timeStamp - keys[i - 1].timeStamp);
            Vector3 position = keys[i - 1].position * (1.0f - time) + keys[i].position * time;
            Matrix4x4 transformation = Matrix4x4::translation(position);

            return transformation;
            // entity->setRotation(keys[i - 1].rotation * (1.0f - time) + keys[i].rotation * time);
            // entity->setScale(keys[i - 1].scale * (1.0f - time) + keys[i].scale * time);
        }
    }
    return Matrix4x4::identity();
}

// picks static
// void getTransformByTimeFixedFrame(float timeStamp, Entity *entity);

void AnimationTarget::addKey(KeyTranform keyTransform)
{
    keys.push_back(keyTransform);
}