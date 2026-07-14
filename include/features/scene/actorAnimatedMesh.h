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
        AnimatedMeshNode(uint64 objectId, const char *name, const char *parentName)
        {
            this->objectId = objectId;
            this->name = std::string(name);
            this->parentName = std::string(parentName);
        }

        inline const char *getName()
        {
            return name.c_str();
        }

        inline const char *getParentName()
        {
            return parentName.c_str();
        }

        inline uint64 getObjectId()
        {
            return objectId;
        }

        std::shared_ptr<Material> material;
        Matrix4x4 transfotmation;
        AnimatedMeshNode *parentNode = nullptr;
        bool isTransformationDirty = true;

    protected:
        uint64 objectId;
        std::string name;
        std::string parentName;
    };

    class WNE_API ActorAnimatedMesh : public Actor
    {
    public:
        ActorAnimatedMesh(Renderer *renderer, std::shared_ptr<MeshCollection> mesh);
        ~ActorAnimatedMesh();

        void setMaterialByName(const char *name, std::shared_ptr<Material> material);
        void setMaterialToAll(std::shared_ptr<Material> material);

        void update(float delta) override final;
        Matrix4x4 getNodeTransformation(AnimatedMeshNode *node);

        void renderDepthShadow(Vector3 &lightPosition) override final;
        void renderDepth() override final;
        void renderColor() override final;

        RenderPass getRenderPass() override final;

        float getBoundingRadius() override final;

        std::shared_ptr<Animation3dTrack> createAnimationTrack(std::shared_ptr<Animation3d> animation = nullptr);

        void removeAnimationTrack(std::shared_ptr<Animation3dTrack> track);

        inline AnimatedMeshNode *getMeshNodeByName(const char *name)
        {
            for (auto &meshNode : nodes)
            {
                if (!strcmp(meshNode.getName(), name))
                    return &meshNode;
            }
            return nullptr;
        }

    protected:
        std::shared_ptr<MeshCollection> mesh;

        // position and data about meshes
        std::vector<AnimatedMeshNode> nodes;
        uint count = 0;

        // animations
        std::vector<std::shared_ptr<Animation3dTrack>> tracks;
    };
};