#pragma once
#include <memory>
#include <core/math.h>

namespace wne
{
    class Sound;

    class SoundSource
    {
    public:
        SoundSource(Sound *sound);
        SoundSource(Sound *sound, bool isLooping);

    protected:
        bool is3d = false;
        bool isPlaying = false;
        bool isLooping = false;
        bool isStreamed = false;
        Sound *sound = nullptr;
        Vector3 source{};
    };
}