#pragma once
#include "core/math.h"

namespace wne
{
    struct VertexColored
    {
        Vector3 pos;
        Vector3 color;
    };

    struct VertexTextured
    {
        Vector3 pos;
        Vector2 uv;
    };
}