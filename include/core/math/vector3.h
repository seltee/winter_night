#pragma once

namespace wne
{
    class Vector3
    {
    public:
        // data
        union
        {
            float x, r, s;
        };
        union
        {
            float y, g, t;
        };
        union
        {
            float z, b, p;
        };

        // initialization
        Vector3(const float x, const float y, const float z) : x(x), y(y), z(z) {}
        Vector3(const float d) : x(d), y(d), z(d) {}
        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

        // operators
        inline Vector3 &operator+=(const Vector3 &rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;
            this->z += rhs.z;
            return *this;
        }
    };

    inline Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs)
    {
        return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }
};
