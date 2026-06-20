#pragma once
#include <memory>
#include "core/math.h"
#include "core/core.h"

namespace wne
{
    class Sound;

    class SoundSource
    {
    public:
        SoundSource(Sound *sound);
        SoundSource(Sound *sound, bool isLooping);

        void addToBuffer(float *buffer, uint amountOfSamples);

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
        Sound *sound = nullptr;
        Vector3 source{};
    };
}