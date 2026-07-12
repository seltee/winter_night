#pragma once
#include "animation3d.h"
#include <memory>

namespace wne
{
    class WNE_API Animation3dTrack
    {
    public:
        Animation3dTrack();
        Animation3dTrack(std::shared_ptr<Animation3d> animation);

        void setAnimation(std::shared_ptr<Animation3d> animation);
        void play();

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
    };
};
