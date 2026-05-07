#pragma once
#include "features/data/model.h"
#include "core/api.h"

namespace wne
{
    class WNE_API Mesh
    {
    public:
        virtual void render(void *frameRenderData);

        inline ModelDataType getDataType()
        {
            return dataType;
        }

    protected:
        ModelDataType dataType = ModelDataType::Unknown;
    };
}