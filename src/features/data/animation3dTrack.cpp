#include "features/data/animation3dTrack.h"

using namespace wne;

Animation3dTrack::Animation3dTrack()
{
}

Animation3dTrack::Animation3dTrack(std::shared_ptr<Animation3d> animation)
{
    this->animation = animation;
}

void Animation3dTrack::setAnimation(std::shared_ptr<Animation3d> animation)
{
    this->animation = animation;
}

void Animation3dTrack::play()
{
}