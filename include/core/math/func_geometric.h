#pragma once
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

namespace wne
{
    inline float dot(const Vector2 &v1, const Vector2 &v2) noexcept
    {
        return v1.x * v2.x + v1.y * v2.y;
    }

    inline float dot(const Vector3 &v1, const Vector3 &v2) noexcept
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    inline float dot(const Vector4 &v1, const Vector4 &v2) noexcept
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
    }

    inline Vector3 cross(const Vector3 &v1, const Vector3 &v2) noexcept
    {
        return Vector3(
            v1.y * v2.z - v1.z * v2.y,
            v1.z * v2.x - v1.x * v2.z,
            v1.x * v2.y - v1.y * v2.x);
    }
};
