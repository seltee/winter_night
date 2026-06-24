#pragma once
#include <memory>
#include "core/math.h"
#include "core/core.h"
#include "features/scene/positionable.h"

namespace wne
{
    class Sound;

    class SoundSource
    {
    public:
        SoundSource(Sound &sound);
        SoundSource(Sound &sound, bool isLooping);

        void addToBuffer(float *buffer, uint amountOfSamples);

        void turn3d(std::shared_ptr<Positionable> listener, const Vector3 &source, float maxDistance);

        inline bool isPlaying()
        {
            return flagIsPlaying;
        }

        inline bool is3d()
        {
            return flagIs3d;
        }

    protected:
        bool flagIs3d = false;
        bool flagIsPlaying = false;
        bool flagIsLooping = false;
        bool flagIsStreamed = false;

        uint soundSampleShift = 0;
        Sound *sound = nullptr;
        Vector3 source{};

        std::shared_ptr<Positionable> listener;
        Vector3 position{};
        float maxDistance = 0.0f;
        float maxDistance2 = 0.0f;
    };
}