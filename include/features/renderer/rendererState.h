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

    protected:
        Matrix4x4 mVP = Matrix4x4::identity();
    };
};
