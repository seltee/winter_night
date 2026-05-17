#pragma once
#include "core/core.h"
#include "core/math.h"
#include "core/api.h"

namespace wne
{
    class WNE_API RendererState
    {
    public:
        virtual ~RendererState();

        inline const Matrix4x4 &getViewProjectionMatrix()
        {
            return mVP;
        }

        inline void setViewProjectionMatrix(const Matrix4x4 &mVP)
        {
            this->mVP = mVP;
        }

        inline const Vector3 &getCameraPosition()
        {
            return cameraPosition;
        }

        inline void setCameraPosition(Vector3 cameraPosition)
        {
            this->cameraPosition = cameraPosition;
        }

    protected:
        Matrix4x4 mVP = Matrix4x4::identity();
        Vector3 cameraPosition{};
    };
};
