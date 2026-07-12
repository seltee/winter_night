#pragma once
#include "animation3d.h"
#include <memory>

namespace wne
{
    class WNE_API Animation3dTrack
    {
    public:
        enum class State
        {
            Pause,
            Playing,
            Looping
        };

        Animation3dTrack();
        Animation3dTrack(std::shared_ptr<Animation3d> animation);

        void update(float delta);

        void setAnimation(std::shared_ptr<Animation3d> animation);
        void play(bool repeat = false);

        inline std::shared_ptr<Animation3d> getAnimation()
        {
            return animation;
        }

        inline void setMixFactor(float mixFactor)
        {
            this->mixFactor = mixFactor;
        }

        inline float getMixFactor()
        {
            return mixFactor;
        }

    protected:
        std::shared_ptr<Animation3d> animation;
        float mixFactor = 1.0f;
        float playPosition = 0.0f;
        float maxPosition = 1.0f;
        State state = State::Pause;
    };
};
