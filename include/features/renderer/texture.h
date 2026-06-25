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

        inline uint32 getWidth() const
        {
            return width;
        }

        inline uint32 getHeight() const
        {
            return height;
        }

    protected:
        uint32 width = 0;
        uint32 height = 0;
    };

};