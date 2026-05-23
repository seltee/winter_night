#pragma once
#include "features/scene/actor.h"
#include "features/renderer/mesh.h"
#include "features/renderer/material.h"
#include "core/api.h"
#include "core/math.h"
#include <memory>

namespace wne
{
    class WNE_API ActorMesh : public Actor
    {
    public:
        ActorMesh(Renderer *renderer, std::shared_ptr<Mesh> mesh);
        ~ActorMesh();

        void setMaterial(std::shared_ptr<Material> material);

        void renderDepthShadow() override final;
        void renderDepth() override final;
        void renderColor() override final;

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
