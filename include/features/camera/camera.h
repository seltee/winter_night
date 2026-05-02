#pragma once
#include "core/math.h"

namespace wne
{
    class Camera
    {
    public:
        const Matrix4x4 &getProjectionMatrix();

    protected:
        Matrix4x4 mProjection;
    };
};
