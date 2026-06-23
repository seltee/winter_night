#include "features/sound/soundSource.h"
#include "features/sound/sound.h"
#include "features/logger/logger.h"
#include <cmath>

using namespace wne;

SoundSource::SoundSource(Sound &sound)
{
    this->sound = &sound;
    this->flagIsPlaying = true;
    position = 0;
}

SoundSource::SoundSource(Sound &sound, bool isLooping)
{
    this->sound = &sound;
    this->flagIsPlaying = true;
    this->flagIsLooping = isLooping;
    position = 0;
}

void SoundSource::addToBuffer(float *buffer, uint amountOfSamples)
{
    if (!flagIsPlaying)
        return;
    uint samplesToPlay = std::min(amountOfSamples, sound->getSampleCount() - position);
    uint samplesToPlay2 = samplesToPlay * 2;
    float *data = sound->getData();

    if (sound->isStereo())
    {
        uint position2 = position * 2;
        for (uint i = 0; i < samplesToPlay2; i += 2)
        {
            buffer[i] = data[i + position2];
            buffer[i + 1] = data[i + position2 + 1];
        }
    }
    else
    {
        for (uint i = 0; i < samplesToPlay2; i += 2)
        {
            float val = data[(i >> 1) + position];
            buffer[i] = val;
            buffer[i + 1] = val;
        }
    }

    position += samplesToPlay;
    if (position >= sound->getSampleCount())
    {
        position = 0;
        if (flagIsLooping)
        {
            // todo should start filling buffer with repeating content
            // addToBuffer(float *buffer, uint amountOfSamples)
        }
        else
        {
            flagIsPlaying = false;
        }
    }
}