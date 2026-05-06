#pragma once
#include "core/api.h"

namespace wne
{
    class WNE_API Mesh
    {
    public:
        virtual void render(void *frameRenderData);
    };
}