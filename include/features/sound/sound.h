#pragma once
#include "core/core.h"
#include <memory>
#include <string>
#include <vector>

namespace wne
{
    class SoundSystem;
    class SoundSource;

    class WNE_API Sound
    {
    public:
        enum class FileFormat
        {
            Unknown,
            WAV,
            OGG
        };

        Sound(SoundSystem *soundSystem, const char *path, bool loadImmidiately, bool isStreaming);
        ~Sound();

        std::shared_ptr<wne::SoundSource> play();
        std::shared_ptr<wne::SoundSource> play(bool loop);

        bool load();

        inline FileFormat getFormat()
        {
            return format;
        }

    protected:
        void detectFormat();
        bool loadWav();

        bool isStreaming = false;
        bool loaded = false;
        bool isStereo = false;
        uint8 bytesPerSample = false;

        std::string path;
        FileFormat format;

        uint dataSize = 0;
        std::vector<float> data;

        SoundSystem *soundSystem = nullptr;
    };
}
