#include "features/sound/soundSystem.h"

using namespace wne;

std::shared_ptr<SoundSource> SoundSystem::playSound(Sound *sound, bool loop)
{
    auto source = std::make_shared<SoundSource>(sound, loop);
    sources.push_back(source);
    return source;
}

std::shared_ptr<SoundSource> SoundSystem::playSound3d(Sound *sound, const Vector3 source, std::shared_ptr<Positionable> listener, bool loop)
{
    return nullptr;
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
}
