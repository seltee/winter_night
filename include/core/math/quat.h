#pragma once
#include <cmath>
#include "matrix4x4.h"

namespace wne
{
    class Quat;
    inline Quat operator*(const Quat &lhs, const Quat &rhs);

    class Quat
    {
    public:
        float x, y, z, w;
        Quat() : x(0), y(0), z(0), w(1.0f) {}
        Quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        // Euler input (radians):
        // Pitch = around X
        // Yaw   = around Y
        // Roll  = around Z
        // roll → pitch → yaw
        Quat(float pitch, float yaw, float roll)
        {
            Quat qPitch = Quat::byAngleInRadians(1.0f, 0.0f, 0.0f, pitch);
            Quat qYaw = Quat::byAngleInRadians(0.0f, 1.0f, 0.0f, yaw);
            Quat qRoll = Quat::byAngleInRadians(0.0f, 0.0f, 1.0f, roll);

            Quat result = qYaw * qPitch * qRoll;
            x = result.x;
            y = result.y;
            z = result.z;
            w = result.w;
        }

        // By angle in radians
        // Should be explicitly normalized
        static Quat byAngleInRadians(float x, float y, float z, float angleRadians)
        {
            float halfAngle = angleRadians / 2.0f;
            float s = std::sin(halfAngle);
            return Quat(
                x * s,
                y * s,
                z * s,
                cosf(halfAngle));
        }

        static Quat aroundWorldX(float angle)
        {
            float half = angle * 0.5;
            float cosHalf = std::cos(half);
            float sinHalf = std::sin(half);
            return {sinHalf, 0.0f, 0.0f, cosHalf};
        }

        static Quat aroundWorldY(float angle)
        {
            float half = angle * 0.5;
            float cosHalf = std::cos(half);
            float sinHalf = std::sin(half);
            return {0.0f, sinHalf, 0.0f, cosHalf};
        }

        static Quat aroundWorldZ(float angle)
        {
            float half = angle * 0.5;
            float cosHalf = std::cos(half);
            float sinHalf = std::sin(half);
            return {0.0, 0.0, sinHalf, cosHalf};
        }
    };

    inline Quat operator*(const Quat &lhs, const Quat &rhs)
    {
        return Quat({
            lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
            lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
            lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z,
        });
    }

    inline Vector3 operator*(const Quat &lhs, const Vector3 &rhs)
    {
        float x = lhs.x, y = lhs.y, z = lhs.z, w = lhs.w;

        float tx = 2.0f * (y * rhs.z - z * rhs.y);
        float ty = 2.0f * (z * rhs.x - x * rhs.z);
        float tz = 2.0f * (x * rhs.y - y * rhs.x);

        return Vector3{
            rhs.x + w * tx + (y * tz - z * ty),
            rhs.y + w * ty + (z * tx - x * tz),
            rhs.z + w * tz + (x * ty - y * tx)};
    }

    inline Matrix4x4 asMatrix(Quat q)
    {
        float xx = q.x * q.x;
        float yy = q.y * q.y;
        float zz = q.z * q.z;
        float xy = q.x * q.y;
        float xz = q.x * q.z;
        float yz = q.y * q.z;
        float wx = q.w * q.x;
        float wy = q.w * q.y;
        float wz = q.w * q.z;

        return Matrix4x4(
            1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
            2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx), 0.0f,
            2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }
};
