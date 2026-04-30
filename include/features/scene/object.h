#pragma once

namespace WNE
{
    class Renderer;

    class Object
    {
    public:
        virtual void render(Renderer *renderer);
    };
}