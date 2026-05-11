#pragma once
#include "core/math.h"

namespace wne
{
    struct VertexColored
    {
        Vector3 pos;
        Vector3 color;
        Vector3 normal;
    };

    struct VertexTextured
    {
        Vector3 pos;
        Vector2 uv;
        Vector3 normal;
    };
}