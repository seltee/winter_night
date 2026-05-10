#pragma once
#include "vector3.h"
#include "vector4.h"
#include <cstring>
#include <cmath>

namespace wne
{
    class Matrix4x4
    {
    public:
        // data
        // stores column-major
        Vector4 m[4];

        // initialization
        Matrix4x4() : m{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}} {}
        // row-major initialization
        Matrix4x4(float v0, float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9, float v10, float v11, float v12, float v13, float v14, float v15)
            : m{{v0, v4, v8, v12},
                {v1, v5, v9, v13},
                {v2, v6, v10, v14},
                {v3, v7, v11, v15}} {}

        // column-major vectors
        Matrix4x4(Vector4 &v0, Vector4 &v1, Vector4 &v2, Vector4 &v3)
            : m{v0, v1, v2, v3} {}

        static Matrix4x4 identity()
        {
            return Matrix4x4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
        }

        static Matrix4x4 translation(float x, float y, float z)
        {
            return Matrix4x4(
                1.0f, 0.0f, 0.0f, x,
                0.0f, 1.0f, 0.0f, y,
                0.0f, 0.0f, 1.0f, z,
                0.0f, 0.0f, 0.0f, 1.0f);
        }

        static Matrix4x4 translation(Vector3 v)
        {
            return Matrix4x4(
                1.0f, 0.0f, 0.0f, v.x,
                0.0f, 1.0f, 0.0f, v.y,
                0.0f, 0.0f, 1.0f, v.z,
                0.0f, 0.0f, 0.0f, 1.0f);
        }

        static Matrix4x4 translation(Vector4 v)
        {
            return Matrix4x4(
                1.0f, 0.0f, 0.0f, v.x / v.w,
                0.0f, 1.0f, 0.0f, v.y / v.w,
                0.0f, 0.0f, 1.0f, v.z / v.w,
                0.0f, 0.0f, 0.0f, 1.0f);
        }

        static Matrix4x4 rotationX(float value)
        {
            float valSin = sinf(value);
            float valCos = cosf(value);
            return Matrix4x4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, valCos, -valSin, 0.0f,
                0.0f, valSin, valCos, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
        }

        static Matrix4x4 rotationY(float value)
        {
            float valSin = sinf(value);
            float valCos = cosf(value);
            return Matrix4x4(
                valCos, 0.0f, valSin, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                -valSin, 0.0f, valCos, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
        }

        static Matrix4x4 rotationZ(float value)
        {
            float valSin = sinf(value);
            float valCos = cosf(value);
            return Matrix4x4(
                valCos, -valSin, 0.0f, 0.0f,
                valSin, valCos, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
        }

        static Matrix4x4 scale(const Vector3 &scale)
        {
            return Matrix4x4(
                scale.x, 0.0f, 0.0f, 0.0f,
                0.0f, scale.y, 0.0f, 0.0f,
                0.0f, 0.0f, scale.z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
        }

        static Matrix4x4 scale(float scaleX, float scaleY, float scaleZ)
        {
            return Matrix4x4(
                scaleX, 0.0f, 0.0f, 0.0f,
                0.0f, scaleY, 0.0f, 0.0f,
                0.0f, 0.0f, scaleZ, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
        }

        static Matrix4x4 scale(float uniformScale)
        {
            return scale(uniformScale, uniformScale, uniformScale);
        }

        static Matrix4x4 rotationAxis(float value, Vector3 axis)
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

            return Matrix4x4(
                valCos + x * axis.x, axay - valSin * axis.z, axaz + valSin * axis.y, 0.0f,
                axay + valSin * axis.z, valCos + y * axis.y, ayaz - valSin * axis.x, 0.0f,
                axaz - valSin * axis.y, ayaz + valSin * axis.x, valCos + z * axis.z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
        }

        // operators
        Matrix4x4 &operator=(const Matrix4x4 &other)
        {
            memcpy(this->m, other.m, sizeof(m));
            return *this;
        }

        Vector4 &operator[](int column)
        {
            return m[column];
        }

        const Vector4 &operator[](int column) const
        {
            return m[column];
        }
    };

    inline Matrix4x4 operator*(const Matrix4x4 &lhs, const Matrix4x4 &rhs)
    {
        return Matrix4x4(
            lhs[0][0] * rhs[0][0] + lhs[1][0] * rhs[0][1] + lhs[2][0] * rhs[0][2] + lhs[3][0] * rhs[0][3],
            lhs[0][0] * rhs[1][0] + lhs[1][0] * rhs[1][1] + lhs[2][0] * rhs[1][2] + lhs[3][0] * rhs[1][3],
            lhs[0][0] * rhs[2][0] + lhs[1][0] * rhs[2][1] + lhs[2][0] * rhs[2][2] + lhs[3][0] * rhs[2][3],
            lhs[0][0] * rhs[3][0] + lhs[1][0] * rhs[3][1] + lhs[2][0] * rhs[3][2] + lhs[3][0] * rhs[3][3],

            lhs[0][1] * rhs[0][0] + lhs[1][1] * rhs[0][1] + lhs[2][1] * rhs[0][2] + lhs[3][1] * rhs[0][3],
            lhs[0][1] * rhs[1][0] + lhs[1][1] * rhs[1][1] + lhs[2][1] * rhs[1][2] + lhs[3][1] * rhs[1][3],
            lhs[0][1] * rhs[2][0] + lhs[1][1] * rhs[2][1] + lhs[2][1] * rhs[2][2] + lhs[3][1] * rhs[2][3],
            lhs[0][1] * rhs[3][0] + lhs[1][1] * rhs[3][1] + lhs[2][1] * rhs[3][2] + lhs[3][1] * rhs[3][3],

            lhs[0][2] * rhs[0][0] + lhs[1][2] * rhs[0][1] + lhs[2][2] * rhs[0][2] + lhs[3][2] * rhs[0][3],
            lhs[0][2] * rhs[1][0] + lhs[1][2] * rhs[1][1] + lhs[2][2] * rhs[1][2] + lhs[3][2] * rhs[1][3],
            lhs[0][2] * rhs[2][0] + lhs[1][2] * rhs[2][1] + lhs[2][2] * rhs[2][2] + lhs[3][2] * rhs[2][3],
            lhs[0][2] * rhs[3][0] + lhs[1][2] * rhs[3][1] + lhs[2][2] * rhs[3][2] + lhs[3][2] * rhs[3][3],

            lhs[0][3] * rhs[0][0] + lhs[1][3] * rhs[0][1] + lhs[2][3] * rhs[0][2] + lhs[3][3] * rhs[0][3],
            lhs[0][3] * rhs[1][0] + lhs[1][3] * rhs[1][1] + lhs[2][3] * rhs[1][2] + lhs[3][3] * rhs[1][3],
            lhs[0][3] * rhs[2][0] + lhs[1][3] * rhs[2][1] + lhs[2][3] * rhs[2][2] + lhs[3][3] * rhs[2][3],
            lhs[0][3] * rhs[3][0] + lhs[1][3] * rhs[3][1] + lhs[2][3] * rhs[3][2] + lhs[3][3] * rhs[3][3]);
    }

    inline Vector4 operator*(const Matrix4x4 &lhs, const Vector4 &rhs)
    {
        return Vector4(
            lhs[0][0] * rhs.x + lhs[1][0] * rhs.y + lhs[2][0] * rhs.z + lhs[3][0] * rhs.w,
            lhs[0][1] * rhs.x + lhs[1][1] * rhs.y + lhs[2][1] * rhs.z + lhs[3][1] * rhs.w,
            lhs[0][2] * rhs.x + lhs[1][2] * rhs.y + lhs[2][2] * rhs.z + lhs[3][2] * rhs.w,
            lhs[0][3] * rhs.x + lhs[1][3] * rhs.y + lhs[2][3] * rhs.z + lhs[3][3] * rhs.w);
    }

    inline float determinant(const Matrix4x4 &m) noexcept
    {
        float submat[3][3];
        float determinant = 0;

        for (int c = 0; c < 4; c++)
        {
            int subi = 0;
            for (int i = 1; i < 4; i++)
            {
                int subj = 0;
                for (int j = 0; j < 4; j++)
                {
                    if (j == c)
                        continue;
                    submat[subi][subj] = m[i][j];
                    subj++;
                }
                subi++;
            }

            float det3x3 = submat[0][0] * (submat[1][1] * submat[2][2] - submat[1][2] * submat[2][1]) -
                           submat[0][1] * (submat[1][0] * submat[2][2] - submat[1][2] * submat[2][0]) +
                           submat[0][2] * (submat[1][0] * submat[2][1] - submat[1][1] * submat[2][0]);

            // Add or subtract the minor determinant (alternating signs)
            determinant += (c % 2 == 0 ? 1 : -1) * m[0][c] * det3x3;
        }
        return determinant;

        return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
               m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
               m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }

    inline Matrix4x4 inverse(const Matrix4x4 &m) noexcept
    {
        float inv[16];

        inv[0] = m[1][1] * m[2][2] * m[3][3] -
                 m[1][1] * m[3][2] * m[2][3] -
                 m[1][2] * m[2][1] * m[3][3] +
                 m[1][2] * m[3][1] * m[2][3] +
                 m[1][3] * m[2][1] * m[3][2] -
                 m[1][3] * m[3][1] * m[2][2];

        inv[4] = -m[0][1] * m[2][2] * m[3][3] +
                 m[0][1] * m[3][2] * m[2][3] +
                 m[0][2] * m[2][1] * m[3][3] -
                 m[0][2] * m[3][1] * m[2][3] -
                 m[0][3] * m[2][1] * m[3][2] +
                 m[0][3] * m[3][1] * m[2][2];

        inv[8] = m[0][1] * m[1][2] * m[3][3] -
                 m[0][1] * m[3][2] * m[1][3] -
                 m[0][2] * m[1][1] * m[3][3] +
                 m[0][2] * m[3][1] * m[1][3] +
                 m[0][3] * m[1][1] * m[3][2] -
                 m[0][3] * m[3][1] * m[1][2];

        inv[12] = -m[0][1] * m[1][2] * m[2][3] +
                  m[0][1] * m[2][2] * m[1][3] +
                  m[0][2] * m[1][1] * m[2][3] -
                  m[0][2] * m[2][1] * m[1][3] -
                  m[0][3] * m[1][1] * m[2][2] +
                  m[0][3] * m[2][1] * m[1][2];

        inv[1] = -m[1][0] * m[2][2] * m[3][3] +
                 m[1][0] * m[3][2] * m[2][3] +
                 m[1][2] * m[2][0] * m[3][3] -
                 m[1][2] * m[3][0] * m[2][3] -
                 m[1][3] * m[2][0] * m[3][2] +
                 m[1][3] * m[3][0] * m[2][2];

        inv[5] = m[0][0] * m[2][2] * m[3][3] -
                 m[0][0] * m[3][2] * m[2][3] -
                 m[0][2] * m[2][0] * m[3][3] +
                 m[0][2] * m[3][0] * m[2][3] +
                 m[0][3] * m[2][0] * m[3][2] -
                 m[0][3] * m[3][0] * m[2][2];

        inv[9] = -m[0][0] * m[1][2] * m[3][3] +
                 m[0][0] * m[3][2] * m[1][3] +
                 m[0][2] * m[1][0] * m[3][3] -
                 m[0][2] * m[3][0] * m[1][3] -
                 m[0][3] * m[1][0] * m[3][2] +
                 m[0][3] * m[3][0] * m[1][2];

        inv[13] = m[0][0] * m[1][2] * m[2][3] -
                  m[0][0] * m[2][2] * m[1][3] -
                  m[0][2] * m[1][0] * m[2][3] +
                  m[0][2] * m[2][0] * m[1][3] +
                  m[0][3] * m[1][0] * m[2][2] -
                  m[0][3] * m[2][0] * m[1][2];

        inv[2] = m[1][0] * m[2][1] * m[3][3] -
                 m[1][0] * m[3][1] * m[2][3] -
                 m[1][1] * m[2][0] * m[3][3] +
                 m[1][1] * m[3][0] * m[2][3] +
                 m[1][3] * m[2][0] * m[3][1] -
                 m[1][3] * m[3][0] * m[2][1];

        inv[6] = -m[0][0] * m[2][1] * m[3][3] +
                 m[0][0] * m[3][1] * m[2][3] +
                 m[0][1] * m[2][0] * m[3][3] -
                 m[0][1] * m[3][0] * m[2][3] -
                 m[0][3] * m[2][0] * m[3][1] +
                 m[0][3] * m[3][0] * m[2][1];

        inv[10] = m[0][0] * m[1][1] * m[3][3] -
                  m[0][0] * m[3][1] * m[1][3] -
                  m[0][1] * m[1][0] * m[3][3] +
                  m[0][1] * m[3][0] * m[1][3] +
                  m[0][3] * m[1][0] * m[3][1] -
                  m[0][3] * m[3][0] * m[1][1];

        inv[14] = -m[0][0] * m[1][1] * m[2][3] +
                  m[0][0] * m[2][1] * m[1][3] +
                  m[0][1] * m[1][0] * m[2][3] -
                  m[0][1] * m[2][0] * m[1][3] -
                  m[0][3] * m[1][0] * m[2][1] +
                  m[0][3] * m[2][0] * m[1][1];

        inv[3] = -m[1][0] * m[2][1] * m[3][2] +
                 m[1][0] * m[3][1] * m[2][2] +
                 m[1][1] * m[2][0] * m[3][2] -
                 m[1][1] * m[3][0] * m[2][2] -
                 m[1][2] * m[2][0] * m[3][1] +
                 m[1][2] * m[3][0] * m[2][1];

        inv[7] = m[0][0] * m[2][1] * m[3][2] -
                 m[0][0] * m[3][1] * m[2][2] -
                 m[0][1] * m[2][0] * m[3][2] +
                 m[0][1] * m[3][0] * m[2][2] +
                 m[0][2] * m[2][0] * m[3][1] -
                 m[0][2] * m[3][0] * m[2][1];

        inv[11] = -m[0][0] * m[1][1] * m[3][2] +
                  m[0][0] * m[3][1] * m[1][2] +
                  m[0][1] * m[1][0] * m[3][2] -
                  m[0][1] * m[3][0] * m[1][2] -
                  m[0][2] * m[1][0] * m[3][1] +
                  m[0][2] * m[3][0] * m[1][1];

        inv[15] = m[0][0] * m[1][1] * m[2][2] -
                  m[0][0] * m[2][1] * m[1][2] -
                  m[0][1] * m[1][0] * m[2][2] +
                  m[0][1] * m[2][0] * m[1][2] +
                  m[0][2] * m[1][0] * m[2][1] -
                  m[0][2] * m[2][0] * m[1][1];

        float det = m[0][0] * inv[0] + m[0][1] * inv[1] + m[0][2] * inv[2] + m[0][3] * inv[3];

        // todo assert for debugging
        // Check if the matrix is degenerated
        if (std::abs(det) < 1e-9)
            return Matrix4x4(
                NAN, NAN, NAN, NAN,
                NAN, NAN, NAN, NAN,
                NAN, NAN, NAN, NAN,
                NAN, NAN, NAN, NAN);

        float invDet = 1.0 / det;

        return Matrix4x4(
            inv[0] * invDet, inv[1] * invDet, inv[2] * invDet, inv[3] * invDet,
            inv[4] * invDet, inv[5] * invDet, inv[6] * invDet, inv[7] * invDet,
            inv[8] * invDet, inv[9] * invDet, inv[10] * invDet, inv[11] * invDet,
            inv[12] * invDet, inv[13] * invDet, inv[14] * invDet, inv[15] * invDet);
    }
};
