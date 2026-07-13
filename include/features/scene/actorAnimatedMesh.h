#pragma once
#include "features/scene/actor.h"
#include "features/renderer/meshCollection.h"
#include "features/renderer/material.h"
#include "features/data/animation3dTrack.h"
#include "core/api.h"
#include "core/math.h"
#include <memory>
#include <vector>

namespace wne
{
    class WNE_API AnimatedMeshNode : public Positionable
    {
    public:
        std::shared_ptr<Material> material;
        uint64 objectId;
        Matrix4x4 transfotmation;
    };

    class WNE_API ActorAnimatedMesh : public Actor
    {
    public:
        ActorAnimatedMesh(Renderer *renderer, std::shared_ptr<MeshCollection> mesh);
        ~ActorAnimatedMesh();

        void update(float delta) override final;

        void renderDepthShadow(Vector3 &lightPosition) override final;
        void renderDepth() override final;
        void renderColor() override final;

        RenderPass getRenderPass() override final;

        float getBoundingRadius() override final;

        std::shared_ptr<Animation3dTrack> createAnimationTrack(std::shared_ptr<Animation3d> animation = nullptr);
        void removeAnimationTrack(std::shared_ptr<Animation3dTrack> track);

    protected:
        std::shared_ptr<MeshCollection> mesh;

        // position and data about meshes
        std::vector<AnimatedMeshNode> nodes;
        uint count = 0;

        // animations
        std::vector<std::shared_ptr<Animation3dTrack>> tracks;
    };
};