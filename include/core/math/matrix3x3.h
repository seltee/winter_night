#pragma once
#include "vector3.h"
#include <cstring>
#include <cmath>

namespace wne
{
    class Matrix4x4;

    class Matrix3x3
    {
    public:
        // data
        // stores column-major
        Vector3 m[3];

        // initialization
        Matrix3x3() : m{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}} {}
        // row-major initialization
        Matrix3x3(float v0, float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8)
            : m{{v0, v3, v6},
                {v1, v4, v7},
                {v2, v5, v8}} {}

        // column-major vectors
        Matrix3x3(Vector3 &v0, Vector3 &v1, Vector3 &v2)
            : m{v0, v1, v2} {}

        Matrix3x3(Matrix4x4 &m4x4);

        static Matrix3x3 identity()
        {
            return Matrix3x3(
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f);
        }

        static Matrix3x3 rotationX(float value)
        {
            float valSin = sinf(value);
            float valCos = cosf(value);
            return Matrix3x3(
                1.0f, 0.0f, 0.0f,
                0.0f, valCos, -valSin,
                0.0f, valSin, valCos);
        }

        static Matrix3x3 rotationY(float value)
        {
            float valSin = sinf(value);
            float valCos = cosf(value);
            return Matrix3x3(
                valCos, 0.0f, valSin,
                0.0f, 1.0f, 0.0f,
                -valSin, 0.0f, valCos);
        }

        static Matrix3x3 rotationZ(float value)
        {
            float valSin = sinf(value);
            float valCos = cosf(value);
            return Matrix3x3(
                valCos, -valSin, 0.0f,
                valSin, valCos, 0.0f,
                0.0f, 0.0f, 1.0f);
        }

        static Matrix3x3 rotationAxis(float value, Vector3 axis)
        {
            float valSin = sinf(value);
            float valCos = cosf(value);
            float d = 1.0f - valCos;
            float x = axis.x * d;
            float y = axis.y * d;
            float z = axis.z * d;
            float axay = x * axis.y;
            float axaz = x * axis.z;
            float ayaz = y * axis.z;

            return Matrix3x3(
                valCos + x * axis.x, axay - valSin * axis.z, axaz + valSin * axis.y,
                axay + valSin * axis.z, valCos + y * axis.y, ayaz - valSin * axis.x,
                axaz - valSin * axis.y, ayaz + valSin * axis.x, valCos + z * axis.z);
        }

        // operators
        Matrix3x3 &operator=(const Matrix3x3 &other)
        {
            memcpy(this->m, other.m, sizeof(m));
            return *this;
        }

        Vector3 &operator[](int column)
        {
            return m[column];
        }

        const Vector3 &operator[](int column) const
        {
            return m[column];
        }
    };

    inline Matrix3x3 operator*(const Matrix3x3 &lhs, const Matrix3x3 &rhs) noexcept
    {
        return Matrix3x3(
            lhs[0][0] * rhs[0][0] + lhs[1][0] * rhs[0][1] + lhs[2][0] * rhs[0][2],
            lhs[0][0] * rhs[1][0] + lhs[1][0] * rhs[1][1] + lhs[2][0] * rhs[1][2],
            lhs[0][0] * rhs[2][0] + lhs[1][0] * rhs[2][1] + lhs[2][0] * rhs[2][2],

            lhs[0][1] * rhs[0][0] + lhs[1][1] * rhs[0][1] + lhs[2][1] * rhs[0][2],
            lhs[0][1] * rhs[1][0] + lhs[1][1] * rhs[1][1] + lhs[2][1] * rhs[1][2],
            lhs[0][1] * rhs[2][0] + lhs[1][1] * rhs[2][1] + lhs[2][1] * rhs[2][2],

            lhs[0][2] * rhs[0][0] + lhs[1][2] * rhs[0][1] + lhs[2][2] * rhs[0][2],
            lhs[0][2] * rhs[1][0] + lhs[1][2] * rhs[1][1] + lhs[2][2] * rhs[1][2],
            lhs[0][2] * rhs[2][0] + lhs[1][2] * rhs[2][1] + lhs[2][2] * rhs[2][2]);
    }

    inline Vector3 operator*(const Matrix3x3 &lhs, const Vector3 &rhs)
    {
        return Vector3(
            lhs[0][0] * rhs.x + lhs[1][0] * rhs.y + lhs[2][0] * rhs.z,
            lhs[0][1] * rhs.x + lhs[1][1] * rhs.y + lhs[2][1] * rhs.z,
            lhs[0][2] * rhs.x + lhs[1][2] * rhs.y + lhs[2][2] * rhs.z);
    }

    inline float determinant(const Matrix3x3 &m) noexcept
    {
        return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
               m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
               m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }

    inline Matrix3x3 inverse(const Matrix3x3 &m) noexcept
    {
        float det = determinant(m);

        // todo assert for debugging
        // Check if the matrix is degenerated
        if (std::abs(det) < 1e-9)
            return Matrix3x3(
                NAN, NAN, NAN,
                NAN, NAN, NAN,
                NAN, NAN, NAN);

        float invDet = 1.0 / det;

        return Matrix3x3(
            (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet,
            (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet,
            (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet,

            (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet,
            (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet,
            (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invDet,

            (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet,
            (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invDet,
            (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invDet);
    }

    inline Matrix3x3 transpose(const Matrix3x3 &m) noexcept
    {
        return Matrix3x3(
            m[0][0], m[0][1], m[0][2],
            m[1][0], m[1][1], m[1][2],
            m[2][0], m[2][1], m[2][2]);
    }
};
