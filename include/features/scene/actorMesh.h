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
        float getBoundingRadius() override final;

        inline uint64 getObjectId()
        {
            return objectId;
        }

        inline void setUVModifier(float shiftX, float shiftY, float scaleX, float scaleY)
        {
            uvModifier.shiftX = shiftX;
            uvModifier.shiftY = shiftY;
            uvModifier.scaleX = scaleX;
            uvModifier.scaleY = scaleY;
        }

    protected:
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
        uint64 objectId = 0xffffffff;

        float r = 0.0f;
        Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};
    };
};
