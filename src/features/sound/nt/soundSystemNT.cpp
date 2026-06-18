#include "features/sound/nt/soundSystemNT.h"

using namespace wne;

std::shared_ptr<SoundSystemNT> SoundSystemNT::create()
{
    return std::make_shared<SoundSystemNT>();
}

void SoundSystemNT::update()
{
}