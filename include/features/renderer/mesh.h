#pragma once
#include "features/data/model.h"
#include "core/api.h"
#include <string>

namespace wne
{
    class WNE_API Mesh
    {
    public:
        virtual void render(void *frameRenderData);

        virtual uint64 genNewObjectId();
        virtual void freeObjectId(uint64 objectId);

        inline float getBoundingRadius()
        {
            return boundingRadius;
        }

        inline ModelDataType getDataType()
        {
            return dataType;
        }

        inline void setName(const char *name)
        {
            this->name = std::string(name);
        }

        inline const char *getName() const
        {
            return name.c_str();
        }

    protected:
        ModelDataType dataType = ModelDataType::Unknown;
        float boundingRadius = 0.0f;
        std::string name;
    };
}