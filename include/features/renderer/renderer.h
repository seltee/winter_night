#pragma once
#include "features/data/modelRender.h"
#include "features/data/model.h"
#include "core/core.h"

namespace WNE
{
    class Renderer
    {
    protected:
    public:
        virtual ~Renderer();
        virtual void render();

        virtual void changeWindowSize(uint32 width, uint32 height);
        virtual ModelRender *createFromModel(Model *model);

        static Renderer *createRendererNT(void *hWnd, uint32 width, uint32 height);

    protected:
        virtual bool setup(uint32 width, uint32 height);
    };
}