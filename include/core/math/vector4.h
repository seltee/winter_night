#pragma once

namespace wne
{
    class Vector4
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
        union
        {
            float w, a, q;
        };

        // initialization
        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        Vector4(float d) : x(d), y(d), z(d), w(d) {}
        Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

        // operators
        Vector4 &operator+=(const Vector4 &rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;
            this->z += rhs.z;
            this->w += rhs.w;
            return *this;
        }

        float &operator[](int index)
        {
            return (&x)[index];
        }

        const float &operator[](int index) const
        {
            return (&x)[index];
        }
    };

    inline Vector4 operator+(const Vector4 &lhs, const Vector4 &rhs)
    {
        return Vector4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
    }
};
