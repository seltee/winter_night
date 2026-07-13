#pragma once
#include "animation3d.h"
#include <memory>
#include <vector>

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
        Animation3dTrack(std::vector<std::shared_ptr<Animation3d>> animations);

        void update(float delta);

        void setAnimations(std::vector<std::shared_ptr<Animation3d>> animations);
        void play(bool repeat = false);

        Matrix4x4 getTransformationMatrix(const char *objectName, float maxMixFactor);

        inline std::vector<std::shared_ptr<Animation3d>> getAnimations()
        {
            return animations;
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
        void recalcMaxPosition();

        std::vector<std::shared_ptr<Animation3d>> animations;
        float mixFactor = 1.0f;
        float playPosition = 0.0f;
        float maxPosition = 1.0f;
        State state = State::Pause;
    };
};
