#pragma once
#include <cmath>
#include "matrix4x4.h"
#include "vector3.h"

namespace wne
{
    class Plane
    {
    public:
        union
        {
            float x, a;
        };
        union
        {
            float y, b;
        };
        union
        {
            float z, c;
        };
        union
        {
            float w, d;
        };

        Plane()
        {
            x = 0;
            y = 0;
            z = 1.0f;
            w = 0.0f;
        }

        Plane(Vector4 data)
        {
            x = data.x;
            y = data.y;
            z = data.z;
            w = data.w;
        }

        Plane(float x, float y, float z, float w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        inline Vector3 getPosition()
        {
            return Vector3(-x * w, -y * w, -z * w);
        }

        inline Vector3 getDirection()
        {
            return Vector3(x, y, z);
        }

        inline void normalize()
        {
            float length = sqrtf(x * x + y * y + z * z);
            x /= length;
            y /= length;
            z /= length;
            w /= length;
        }
    };
};