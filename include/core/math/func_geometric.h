#pragma once
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "quat.h"

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

    inline Quat normalize(const Quat &q)
    {
        float norm = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
        if (norm < 1e-12)
            return {1.0, 0.0, 0.0, 0.0};
        return {q.x / norm, q.y / norm, q.z / norm, q.w / norm};
    }

    inline Vector3 normalize(const Vector3 &vec)
    {
        float norm = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
        if (norm < 1e-12)
            return {1.0, 0.0, 0.0};
        return {vec.x / norm, vec.y / norm, vec.z / norm};
    }

    inline float wrap2Pi(float angle)
    {
        angle = std::fmod(angle, PI2);
        if (angle < 0)
            angle += PI2;
        return angle;
    }

    inline Matrix4x4 makePerspectiveProjectionMatrix(float aspectRatio, float fov, float nearPlane, float farPlane)
    {
        float f = 1.0f / tanf(fov * 0.5f);
        float nf = farPlane - nearPlane;

        return Matrix4x4(
            f / aspectRatio, 0.0f, 0.0f, 0.0f,
            0.0f, -f, 0.0f, 0.0f,
            0.0f, 0.0f, farPlane / nf, -nearPlane * farPlane / nf,
            0.0f, 0.0f, 1.0f, 0.0f);
    }

    inline Matrix4x4 makeOrthographicProjectionMatrix(float left, float right, float top, float bottom, float near, float far)
    {
        return Matrix4x4(
            2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
            0.0f, 2.0f / (top - bottom), 0.0f, (top + bottom) / (top - bottom),
            0.0f, 0.0f, 1.0f / (far - near), -near / (far - near),
            0.0f, 0.0f, 0.0f, 1.0f);
    }

};
