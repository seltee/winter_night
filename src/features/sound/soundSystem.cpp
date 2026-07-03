#include "features/sound/soundSystem.h"
#include "features/logger/logger.h"

using namespace wne;

std::shared_ptr<SoundSystem> SoundSystem::create()
{
    return std::make_shared<SoundSystem>();
}

std::shared_ptr<SoundSource> SoundSystem::playSound(Sound &sound, bool loop)
{
    auto source = std::make_shared<SoundSource>(sound, loop);
    sources.push_back(source);
    return source;
}

std::shared_ptr<SoundSource> SoundSystem::playSound3d(Sound &sound, const Vector3 &position, std::shared_ptr<Positionable> listener, bool loop)
{
    auto source = std::make_shared<SoundSource>(sound, loop);
    source->turn3d(std::move(listener), position, 121.0f);
    sources.push_back(source);
    return source;
}

std::shared_ptr<SoundSource> SoundSystem::playSound3d(Sound &sound, const Vector3 &position, std::shared_ptr<Positionable> listener, float maxDistance, bool loop)
{
    auto source = std::make_shared<SoundSource>(sound, loop);
    source->turn3d(std::move(listener), position, maxDistance);
    sources.push_back(source);
    return source;
}

std::shared_ptr<Sound> SoundSystem::loadSound(const char *path)
{
    return std::make_shared<Sound>(this, path, true, false);
}

std::shared_ptr<Sound> SoundSystem::loadMusic(const char *path)
{
    return std::make_shared<Sound>(this, path, true, true);
}

void SoundSystem::update()
{
    updateBuffers();
}

void SoundSystem::updateBuffers()
{
}

void SoundSystem::provideBuffer(float *data, uint sampleCount)
{
}

void SoundSystem::fillBuffer(float *data, uint32 sampleCount)
{
    memset(data, 0, sampleCount * sizeof(float) * 2);

    for (auto &it : sources)
    {
        if (it->isPlaying())
        {
            it->addToBuffer(data, sampleCount);
        }
    }

    // Limiter
    float fullSize = sampleCount * 2;
    for (uint i = 0; i < fullSize; i++)
    {
        data[i] = fminf(fmaxf(data[i], -1.0f), 1.0f);
    }

    provideBuffer(data, sampleCount);
}