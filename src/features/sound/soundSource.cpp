#include "features/sound/soundSource.h"

using namespace wne;

SoundSource::SoundSource(Sound *sound)
{
    this->sound = sound;
    this->isPlaying = true;
}

SoundSource::SoundSource(Sound *sound, bool isLooping)
{
    this->sound = sound;
    this->isPlaying = true;
    this->isLooping = isLooping;
}