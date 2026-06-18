#pragma once
#include <memory>
#include <vector>
#include "features/sound/soundSystem.h"

namespace wne
{
    class SoundSystemNT : public SoundSystem
    {
    public:
        static std::shared_ptr<SoundSystemNT> create();

        virtual void update();

    protected:
    };
}
