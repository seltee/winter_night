#pragma once
#include "core/core.h"

namespace wne
{
    class Renderer;
    class WNE_API UINode
    {
    public:
        struct Context
        {
            Renderer *renderer;
        };
        

        UINode(const UINode &) = delete;
        UINode &operator=(const UINode &) = delete;

        UINode();
        virtual ~UINode();
        virtual void update(int x, int y, uint width, uint height);
        virtual void render(Context &context);

        virtual uint getWidth();
        virtual uint getHeight();
    };
};