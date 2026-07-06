#pragma once
#include "features/scene/actor.h"
#include "features/renderer/mesh.h"
#include "features/renderer/material.h"
#include "core/api.h"
#include "core/math.h"
#include <memory>

namespace wne
{
    class WNE_API ActorSprite : public Actor
    {
    public:
        enum class ShadowRenderingMode
        {
            FromCamera, // sprite rotated to camera when shadow is rendered
            FromLight   // sprite rotated to light when shadow is rendered
        };

        ActorSprite(Renderer *renderer);
        ~ActorSprite();

        void setMaterial(std::shared_ptr<Material> material);

        const Matrix4x4 &getModelMatrix();

        void update(float delta) override final;
        void renderDepthShadow() override final;
        void renderDepth() override final;
        void renderColor() override final;

        void setShadowRenderingMode(ShadowRenderingMode shadowRenderingMode);
        void setFrame(uint frame);
        void updateUV();

        RenderPass getRenderPass() override final;

        float getBoundingRadius() override final;

        inline uint64 getObjectId()
        {
            return objectId;
        }

        inline void setFramesDimensions(uint32 framesHorizontal, uint32 framesVertical)
        {
            this->framesHorizontal = framesHorizontal;
            this->framesVertical = framesVertical;
        }

        inline void setFramesHorizontal(uint32 framesHorizontal)
        {
            this->framesHorizontal = framesHorizontal;
        }

        inline uint32 getFramesHorizontal()
        {
            return framesHorizontal;
        }

        inline void setFramesVertical(uint32 framesVertical)
        {
            this->framesVertical = framesVertical;
        }

        inline uint32 getFramesVertical()
        {
            return framesVertical;
        }

        inline uint getFrame()
        {
            return frame;
        }

    protected:
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
        uint64 objectId = 0xffffffff;

        float r = 0.0f;

        ShadowRenderingMode shadowRenderingMode = ShadowRenderingMode::FromCamera;
        uint32 framesHorizontal = 1;
        uint32 framesVertical = 1;
        uint frame = 0;

        Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};
    };
};
