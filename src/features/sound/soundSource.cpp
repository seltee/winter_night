#include "features/sound/soundSource.h"
#include "features/sound/sound.h"
#include "features/logger/logger.h"
#include <cmath>

using namespace wne;

SoundSource::SoundSource(Sound *sound)
{
    this->sound = sound;
    this->flagIsPlaying = true;
}

SoundSource::SoundSource(Sound *sound, bool isLooping)
{
    this->sound = sound;
    this->flagIsPlaying = true;
    this->flagIsLooping = isLooping;
}

void SoundSource::addToBuffer(float *buffer, uint amountOfSamples)
{
    // uint addSamples = min(amountOfSamples, this->sound);
    for (uint i = 0; i < amountOfSamples; i++)
    {
        buffer[(i << 1) + 0] += sin((float)(i) * 0.01f) * 0.5f;
        buffer[(i << 1) + 1] += sin((float)(i) * 0.01f) * 0.5f;
    }
}