#pragma once
#include "features/renderer/texture.h"
#include "features/data/model.h"
#include "core/math.h"
#include "core/api.h"
#include <memory>

namespace wne
{
    class Renderer;
    struct AffectingLights
    {
        uint32 lightsAmount;
        uint32 lightIds[12];
    };

    class WNE_API Material
    {
    public:
        virtual void rebuild();

        // bind for light's shadow depth
        virtual void bindDepthShadow(uint64 objectId, Renderer *renderer, const Matrix4x4 &mMVP, const Matrix3x3 &mNormal, ModelDataType dataType);

        // bind for depth render
        virtual void bindDepth(uint64 objectId, const Matrix4x4 &mMVP, const Matrix4x4 &mModel, const Matrix3x3 &mNormal, ModelDataType dataType);

        // bind for color render
        virtual void bindColor(uint64 objectId, const AffectingLights &lights, const Matrix4x4 &mMVP, const Matrix4x4 &mModel, const Matrix3x3 &mNormal, ModelDataType dataType);

        void setMasked(bool masked)
        {
            flagIsMasked = masked;
        }

        bool isMasked()
        {
            return flagIsMasked;
        }

    protected:
        bool flagIsMasked = false;
    };
};