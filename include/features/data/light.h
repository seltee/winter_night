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

        virtual void updateTransformation(const Matrix4x4 &mModel);
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

        inline void setAffectRadius(float affectRadius)
        {
            this->affectRadius = affectRadius;
        }

        inline float getAffectRadius()
        {
            return affectRadius;
        }

        inline uint32 getLightId()
        {
            return lightId;
        }

        inline Vector4 getPosition()
        {
            return position;
        }

        inline Vector4 getRealDirection()
        {
            return realDirection;
        }

    protected:
        Type type;

        Vector4 direction = {0, -1.0f, 0, 1.0f};     // provided by user
        Vector4 realDirection = {0, -1.0f, 0, 1.0f}; // calculated based on owner's transformation matrix
        Vector4 position = {0, 0, 0, 1.0f};          // calculated based on owner's transformation matrix
        float affectRadius = 1.0f;

        Vector4 color = {0.5f, 0.5f, 0.5f, 1.0f};
        uint32 lightId = 0xffffffff;
    };
};
