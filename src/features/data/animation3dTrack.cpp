#include "features/data/animation3dTrack.h"
#include "features/logger/logger.h"

using namespace wne;

Animation3dTrack::Animation3dTrack()
{
}

Animation3dTrack::Animation3dTrack(std::shared_ptr<Animation3d> animation)
{
    this->animation = animation;
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

void Animation3dTrack::setAnimation(std::shared_ptr<Animation3d> animation)
{
    this->animation = animation;
    recalcMaxPosition();
}

void Animation3dTrack::play(bool repeat)
{
    this->state = repeat ? State::Looping : State::Playing;
}

Matrix4x4 Animation3dTrack::getTransformationMatrix(const char *objectName, float maxMixFactor)
{
    auto animationTarget = animation->getAnimationTarget(objectName);
    if (animationTarget)
    {
        return animationTarget->getTransformByTime(playPosition);
    }

    return Matrix4x4::identity();
}

void Animation3dTrack::recalcMaxPosition()
{
    maxPosition = animation->getAnimationLength();
}