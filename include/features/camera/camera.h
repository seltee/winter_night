#pragma once
#include "core/math.h"

namespace wne
{
    class Camera
    {
    public:
        const Matrix4x4 &getProjectionMatrix();
        virtual void update();

    protected:
        Matrix4x4 mProjection;
    };
};
