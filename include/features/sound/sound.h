#pragma once
#include "core/core.h"
#include "features/scene/positionable.h"
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

        std::shared_ptr<SoundSource> play();
        std::shared_ptr<SoundSource> play(bool loop);
        std::shared_ptr<SoundSource> play3d(std::shared_ptr<Positionable> listener, const Vector3 &source);
        std::shared_ptr<SoundSource> play3d(std::shared_ptr<Positionable> listener, const Vector3 &source, bool loop);

        bool load();

        inline uint getSampleCount()
        {
            return sampleCount;
        }

        inline float *getData()
        {
            return data.data();
        }

        inline bool isStereo()
        {
            return flagIsStereo;
        }

        inline bool isStreaming()
        {
            return flagIsStreaming;
        }

        inline FileFormat getFormat()
        {
            return format;
        }

    protected:
        void detectFormat();
        bool loadWav();

        bool flagIsStreaming = false;
        bool flagIsStereo = false;
        bool loaded = false;

        uint sampleCount = 0;

        std::string path;
        FileFormat format;

        uint dataSize = 0;
        std::vector<float> data;

        SoundSystem *soundSystem = nullptr;
    };
}
