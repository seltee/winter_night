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
            float lTime = 1.0f - time;
            Vector3 position = keys[i - 1].position * lTime + keys[i].position * time;
            Vector3 pRotation = keys[i - 1].rotation;
            Vector3 nRotation = keys[i].rotation;
            Vector3 scale = keys[i - 1].scale * lTime + keys[i].scale * time;

            auto q0 = Quat(pRotation.x, pRotation.y, pRotation.z);
            auto q1 = Quat(nRotation.x, nRotation.y, nRotation.z);

            auto q = slerp(q0, q1, time);

            Matrix4x4 transformation = Matrix4x4::translation(position);
            transformation = transformation * asMatrix(q);
            transformation = transformation * Matrix4x4::scale(scale);

            return transformation;
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