#pragma once
#include "vector3.h"
#include <cstring>
#include <iostream>
#include <cmath>

namespace wne
{
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

        static Matrix3x3 identity()
        {
            return Matrix3x3(
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f);
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

    Matrix3x3 operator*(const Matrix3x3 &lhs, const Matrix3x3 &rhs) noexcept
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

    inline float determinant(const Matrix3x3 &m) noexcept
    {
        return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
               m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
               m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }

    Matrix3x3 inverse(const Matrix3x3 &m) noexcept
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
};
