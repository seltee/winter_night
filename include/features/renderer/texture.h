#pragma once
#include <memory>
#include "features/data/image.h"

namespace wne
{
    class Texture
    {
    public:
        virtual bool setup(void *pixels, uint32 width, uint32 height);
        virtual void bind();
    };

};