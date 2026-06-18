#pragma once
#include <memory>
#include <vector>
#include "features/sound/sound.h"
#include "features/sound/soundSource.h"
#include "features/scene/positionable.h"
#include "core/core.h"

namespace wne
{
    class WNE_API SoundSystem
    {
    public:
        std::shared_ptr<SoundSource> playSound(Sound *sound, bool loop = false);
        std::shared_ptr<SoundSource> playSound3d(Sound *sound, const Vector3 source, std::shared_ptr<Positionable> listener, bool loop = false);
        std::shared_ptr<Sound> loadSound(const char *path);
        std::shared_ptr<Sound> loadMusic(const char *path);

        virtual void update();

    protected:
        std::vector<std::shared_ptr<SoundSource>> sources;
    };
}
