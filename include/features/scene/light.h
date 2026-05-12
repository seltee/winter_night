#pragma once
#include "core/core.h"
#include "core/math.h"
#include "core/api.h"

namespace wne
{
    class WNE_API Light
    {
    public:
        enum class Type
        {
            Directional,
            Omni,
            Spot
        };
        Light(Type type);

        virtual void prepareForRender();

        inline void setColor(Vector3 color)
        {
            this->color = Vector4(color, 1.0f);
        }

        inline Vector4 getColor()
        {
            return color;
        }

        inline void setDirection(Vector3 direction)
        {
            this->direction = Vector4(normalize(direction), 1.0f);
        }

        inline Vector4 getDirection()
        {
            return direction;
        }

        inline Type getType()
        {
            return type;
        }

        inline uint32 getLightId()
        {
            return lightId;
        }

    protected:
        Type type;
        Vector4 direction = {0, -1.0f, 0, 1.0f};
        Vector4 color = {0.5f, 0.5f, 0.5f, 1.0f};
        uint32 lightId = 0xffffffff;
    };
};
