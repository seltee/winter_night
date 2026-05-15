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
        ActorMesh(std::shared_ptr<Mesh> mesh);
        ~ActorMesh();
        static std::shared_ptr<ActorMesh> create(std::shared_ptr<Mesh> mesh);

        void setMaterial(std::shared_ptr<Material> material);

        void renderDepth(Renderer *renderer) override final;
        void renderColor(Renderer *renderer) override final;

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
