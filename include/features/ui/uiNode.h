#pragma once
#include "core/core.h"

namespace wne
{
    class UINode
    {
        public:
            virtual void update(uint16 width, uint16 height);
            virtual void render();
    };
};