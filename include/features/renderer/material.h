#pragma once
#include "features/renderer/texture.h"
#include "features/renderer/meshArmature.h"
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
        struct UVData
        {
            float shiftX, shiftY;
            float scaleX, scaleY;
        };

        virtual void rebuild();

        // bind for light's shadow depth
        virtual void bindDepthShadow(
            uint64 objectId,
            Renderer *renderer,
            const Matrix4x4 &mMVP,
            const Matrix3x3 &mNormal,
            const UVData &uvData,
            const MeshArmature *meshArmature,
            bool isDoubleSided,
            ModelDataType dataType);

        // bind for depth render
        virtual void bindDepth(
            uint64 objectId,
            const Matrix4x4 &mMVP,
            const Matrix4x4 &mModel,
            const Matrix3x3 &mNormal,
            const UVData &uvData,
            const MeshArmature *meshArmature,
            ModelDataType dataType);

        // bind for color render
        virtual void bindColor(
            uint64 objectId,
            const AffectingLights &lights,
            const Matrix4x4 &mMVP,
            const Matrix4x4 &mModel,
            const Matrix3x3 &mNormal,
            const UVData &uvData,
            const MeshArmature *meshArmature,
            ModelDataType dataType);

        void setMasked(bool masked)
        {
            flagIsMasked = masked;
        }

        bool isMasked()
        {
            return flagIsMasked;
        }

        void setLighted(bool lighted)
        {
            flagIsLighted = lighted;
        }

        bool isLighted()
        {
            return flagIsLighted;
        }

        void setColorBlending(ColorBlending colorBlending)
        {
            this->colorBlending = colorBlending;
        }

        // 0 - full normal effect, 1 - full light, ignoring polygon's normal
        // use it to make the light to go through polygons
        void setNormalShadowingFactor(float normalShadowingFactor = 0.0f)
        {
            this->normalShadowingFactor = normalShadowingFactor;
        }

        float getNormalShadowingFactor()
        {
            return normalShadowingFactor;
        }

        ColorBlending getColorBlending()
        {
            return colorBlending;
        }

    protected:
        bool flagIsMasked = false;
        bool flagIsLighted = true;

        ColorBlending colorBlending = ColorBlending::Solid;
        float normalShadowingFactor = 0.0f;
    };
};