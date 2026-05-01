#pragma once

namespace wne
{
    class Renderer;

    class Object
    {
    public:
        virtual void render(Renderer *renderer);
    };
}