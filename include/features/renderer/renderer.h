#pragma once

namespace WNE
{
    class Renderer
    {
    protected:
    public:
        virtual ~Renderer();
        virtual void render();

        virtual void changeWindowSize(int width, int height);

        static Renderer *createRendererNT(void *hWnd, int width, int height);

    protected:
        virtual bool setup(int width, int height);
    };
}