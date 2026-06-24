#include "features/sound/sound.h"
#include "features/sound/soundHelpers.h"
#include "features/sound/soundSystem.h"
#include "features/logger/logger.h"
#include <cstring>
#include <cstdlib>
#include <iostream>

using namespace wne;

Sound::Sound(SoundSystem *soundSystem, const char *path, bool loadImmidiately, bool isStreaming)
{
    this->soundSystem = soundSystem;
    this->path = std::string(path);
    this->flagIsStreaming = isStreaming;
    detectFormat();

    if (loadImmidiately)
        load();
}

Sound::~Sound()
{
}

std::shared_ptr<SoundSource> Sound::play()
{
    return soundSystem->playSound(*this, false);
}

std::shared_ptr<SoundSource> Sound::play(bool loop)
{
    return soundSystem->playSound(*this, loop);
}

std::shared_ptr<SoundSource> Sound::play3d(std::shared_ptr<Positionable> listener, const Vector3 &source)
{
    return soundSystem->playSound3d(*this, source, listener, false);
}

std::shared_ptr<SoundSource> Sound::play3d(std::shared_ptr<Positionable> listener, const Vector3 &source, bool loop)
{
    return soundSystem->playSound3d(*this, source, listener, loop);
}

bool Sound::load()
{
    if (flagIsStreaming)
        return false;
    if (loaded)
        return true;

    if (format == FileFormat::WAV && loadWav())
    {
        loaded = true;
        return true;
    }

    return false;
}

void Sound::detectFormat()
{
    format = FileFormat::Unknown;
    int length = path.length();
    if (length > 5)
    {
        if (
            (path[length - 1] == 'v' || path[length - 1] == 'V') &&
            (path[length - 2] == 'a' || path[length - 2] == 'A') &&
            (path[length - 3] == 'w' || path[length - 3] == 'W') &&
            path[length - 4] == '.')
        {
            format = FileFormat::WAV;
        }

        if (
            (path[length - 1] == 'g' || path[length - 1] == 'G') &&
            (path[length - 2] == 'g' || path[length - 2] == 'G') &&
            (path[length - 3] == 'o' || path[length - 3] == 'O') &&
            path[length - 4] == '.')
        {
            format = FileFormat::OGG;
        }
    }
}

bool Sound::loadWav()
{
    FILE *file = fopen(path.c_str(), "rb");
    if (file == nullptr)
    {
        printf("unable to open file %s\n", path.c_str());
        return false;
    }

    WavHeader wavHeader;
    fread(&wavHeader, sizeof(WavHeader), 1, file);

    // printf("wav loader - %s\nformat %i, num of channels %i, sampleRate %i, byteRate %i, bytesPerSample %i, bitsPerSample %i\n",
    //       path.c_str(), wavHeader.format, wavHeader.numOfChannels, wavHeader.sampleRate, wavHeader.byteRate, wavHeader.bytesPerSample,
    //       wavHeader.bitsPerSample);

    int bitsPerSample = wavHeader.bitsPerSample;
    int numOfChannels = wavHeader.numOfChannels;

    // Looking for wav data chapter
    WavChapterHeader wch;
    while (true)
    {
        fread(&wch, sizeof(WavChapterHeader), 1, file);
        if (wch.data[0] == 'd' && wch.data[1] == 'a' && wch.data[2] == 't' && wch.data[3] == 'a')
        {
            dataSize = wch.dataSize;
            break;
        }
        else
        {
            fseek(file, wch.dataSize, SEEK_CUR);
        }

        // Data section wasn't found
        if (feof(file))
        {
            fclose(file);
            return false;
        }
    }

    // Buffer to store the whole file
    std::vector<uint8> rawData;
    rawData.resize(dataSize);
    fread(rawData.data(), dataSize, 1, file);
    fclose(file);

    bool bIsUsingFloat = wavHeader.format == 3;
    if (bIsUsingFloat)
    {
        if (numOfChannels == 2)
        {
            sampleCount = dataSize / 8;
            for (uint i = 0; i < sampleCount; i++)
            {
                data.push_back(*((float *)(&rawData.data()[i * 8])));
                data.push_back(*((float *)(&rawData.data()[i * 8 + 4])));
            }
            this->sampleCount = sampleCount;
            flagIsStereo = true;
        }
        else
        {
            sampleCount = dataSize / 4;
            for (uint i = 0; i < sampleCount; i++)
                data.push_back(*((float *)(&rawData.data()[i * 4])));
            flagIsStereo = false;
        }
    }
    else
    {
        Logger::log << "Only 44100hz float format mono/stereo is supported" << endl;
        return false;
    }
    return true;
}