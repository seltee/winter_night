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
    };
}