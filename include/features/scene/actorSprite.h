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
        void setFrameSize(float frameWidth, float frameHeight);
        void setFrame(uint frame);

        RenderPass getRenderPass() override final;

        float getBoundingRadius() override final;

        inline uint64 getObjectId()
        {
            return objectId;
        }

        inline float getFrameWidth()
        {
            return frameWidth;
        }

        inline float getFrameHeight()
        {
            return frameHeight;
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
        float frameWidth = 1.0f;
        float frameHeight = 1.0f;
        uint frame = 0;
    };
};
