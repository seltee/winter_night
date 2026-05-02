#pragma once

namespace wne
{
    class Renderer;

    class Object
    {
    public:
        virtual void update(float delta);
        virtual void render(Renderer *renderer);
    };
}