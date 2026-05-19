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
        ActorSprite(Renderer *renderer);
        ~ActorSprite();
        static std::shared_ptr<ActorSprite> create(Renderer *renderer);

        void setMaterial(std::shared_ptr<Material> material);

        const Matrix4x4 &getModelMatrix();

        void update(float delta) override final;
        void renderDepthShadow(Renderer *renderer) override final;
        void renderDepth(Renderer *renderer) override final;
        void renderColor(Renderer *renderer) override final;

        RenderPass getRenderPass() override final;

        inline uint64 getObjectId()
        {
            return objectId;
        }

    protected:
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
        uint64 objectId = 0xffffffff;

        float r = 0.0f;
    };
};
