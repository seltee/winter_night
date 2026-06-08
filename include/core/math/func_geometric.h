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
        float norm = std::sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
        if (norm < 1e-12)
            return {1.0, 0.0, 0.0, 0.0};
        return {q.x / norm, q.y / norm, q.z / norm, q.w / norm};
    }

    inline Vector3 normalize(const Vector3 &vec)
    {
        float norm = std::sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
        if (norm < 1e-12)
            return {0.0, 0.0, -1.0};
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
            0.0f, -2.0f / (top - bottom), 0.0f, (top + bottom) / (top - bottom),
            0.0f, 0.0f, 1.0f / (far - near), -near / (far - near),
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    inline Matrix3x3 lookAt(const Vector3 &from, const Vector3 &to)
    {
        const Vector3 up = Vector3(0, 1, 0);
        Vector3 forward = normalize(to - from);

        if (std::abs(dot(forward, up)) > 0.9999f)
        {
            Vector3 alternativeUp = std::abs(dot(forward, Vector3(0, 0, 1))) > 0.9999f
                                        ? Vector3(0, 1, 0)
                                        : Vector3(0, 0, 1);

            Vector3 right = normalize(cross(alternativeUp, forward));
            Vector3 newUp = cross(forward, right);

            return Matrix3x3(right, newUp, forward);
        }

        Vector3 right = normalize(cross(up, forward));
        Vector3 newUp = cross(forward, right);

        Matrix3x3 rot(right, newUp, forward);

        return rot;
    }

    inline float distance(const Vector3 &from, const Vector3 &to)
    {
        float difX = to.x - from.x;
        float difY = to.y - from.y;
        float difZ = to.z - from.z;
        return sqrtf(difX * difX + difY * difY + difZ * difZ);
    }

    inline float distance(const Plane &plane, const Vector3 &point)
    {
        return plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.w;
    }

    inline float getHighestAxisValue(const Vector4 &vec)
    {
        return fmaxf(fmaxf(fmaxf(vec.x, vec.y), vec.z), vec.w);
    }

    inline float getHighestAxisValue(const Vector3 &vec)
    {
        return fmaxf(fmaxf(vec.x, vec.y), vec.z);
    }

    inline float getHighestAxisValue(const Vector2 &vec)
    {
        return fmaxf(vec.x, vec.y);
    }

};
