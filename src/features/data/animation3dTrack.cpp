#include "features/data/animation3dTrack.h"
#include "features/logger/logger.h"

using namespace wne;

Animation3dTrack::Animation3dTrack()
{
}

Animation3dTrack::Animation3dTrack(std::vector<std::shared_ptr<Animation3d>> animations)
{
    this->animations = animations;
    recalcMaxPosition();
}

void Animation3dTrack::update(float delta)
{
    if (state == State::Playing || state == State::Looping)
    {
        playPosition += delta;
        if (playPosition > maxPosition)
        {
            if (state == State::Playing)
            {
                playPosition = 0.0f;
                state = State::Pause;
            }
            else if (state == State::Looping)
            {
                playPosition -= maxPosition;
            }
        }
    }
}

void Animation3dTrack::setAnimations(std::vector<std::shared_ptr<Animation3d>> animations)
{
    this->animations = animations;
    recalcMaxPosition();
}

void Animation3dTrack::play(bool repeat)
{
    this->state = repeat ? State::Looping : State::Playing;
}

Matrix4x4 Animation3dTrack::getTransformationMatrix(const char *objectName, float maxMixFactor)
{
    if (!animations.size())
        return Matrix4x4::identity();

    Logger::log << "Get for " << objectName << " count " << animations.size() << endl;

    float localFactor = mixFactor / maxMixFactor;

    for (auto &animation : animations)
    {
        auto animationTarget = animation->getAnimationTarget(objectName);
        if (animationTarget)
        {
            Logger::log << "Found for " << animation->getName() << " " << animationTarget->getKeysCount() << endl;
            return animationTarget->getTransformByTime(playPosition);
        }
        Logger::log << "Animation target is for " << animationTarget->getTargetName().c_str() << endl;
    }

    return Matrix4x4::identity();
}

void Animation3dTrack::recalcMaxPosition()
{
    maxPosition = 0.0f;
    for (auto &animation : animations)
        maxPosition = std::max(animation->getAnimationLength(), maxPosition);
}