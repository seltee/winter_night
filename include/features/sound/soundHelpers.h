#pragma once
#include "core/core.h"

namespace wne
{
    struct WavHeader
    {
        char mark[4];
        int fileSize;
        char wave[4];
        char fmt[4];
        int formatLength;
        uint16 format;
        uint16 numOfChannels;
        int sampleRate;
        int byteRate;
        uint16 bytesPerSample;
        uint16 bitsPerSample;
    };

    struct WavChapterHeader
    {
        char data[4];
        uint dataSize;
    };
}
