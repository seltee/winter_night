#include "features/data/animation3dTrack.h"
#include "features/logger/logger.h"

using namespace wne;

Animation3dTrack::Animation3dTrack()
{
}

Animation3dTrack::Animation3dTrack(std::shared_ptr<Animation3d> animation)
{
    this->animation = animation;
    maxPosition = animation->getAnimationLength();
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
}

void Animation3dTrack::play(bool repeat)
{
    this->state = repeat ? State::Looping : State::Playing;
}