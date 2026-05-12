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

        inline uint64 getObjectId()
        {
            return objectId;
        }

    protected:
        ModelDataType dataType = ModelDataType::Unknown;
        uint64 objectId = 0xffffffff;
    };
}