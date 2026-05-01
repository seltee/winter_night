#pragma once

namespace wne
{
    class Vector2
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

        // initialization
        Vector2(float x, float y) : x(x), y(y) {}
        Vector2(float d) : x(d), y(d) {}
        Vector2() : x(0.0f), y(0.0f) {}

        // operators
        Vector2 &operator+=(const Vector2 &rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;
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

    inline Vector2 operator-(const Vector2 &lhs, const Vector2 &rhs)
    {
        return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
    }
};
