#pragma once
#include "features/scene/actor.h"
#include "features/renderer/meshCollection.h"
#include "features/renderer/material.h"
#include "features/renderer/meshArmature.h"
#include "features/data/animation3dTrack.h"
#include "features/logger/logger.h"
#include "features/renderer/renderer.h"
#include "core/api.h"
#include "core/math.h"
#include <memory>
#include <vector>

namespace wne
{
    class AnimatedMeshArmatureNode;
    class WNE_API AnimatedMeshNode : public Positionable
    {
    public:
        AnimatedMeshNode(uint64 objectId, const char *name, const char *parentName, bool isEmptyFlag);

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

        inline bool isEmpty()
        {
            return isEmptyFlag;
        }

        std::shared_ptr<Material> material;
        Matrix4x4 transfotmation;
        AnimatedMeshNode *parentNode = nullptr;
        float boundingRadius = 1.0f;
        Vector3 position{0, 0, 0};
        bool isTransformationDirty = true;
        bool isInRender = false;
        bool isEmptyFlag = false;
        std::shared_ptr<AnimatedMeshArmatureNode> armature;

    protected:
        uint64 objectId;
        std::string name;
        std::string parentName;
    };

    class AnimatedMeshArmatureNode
    {
    public:
        struct BoneBinding
        {
            uint matrixIndex;
            AnimatedMeshNode *meshNode;
        };

        AnimatedMeshArmatureNode(Renderer *renderer, std::shared_ptr<Armature> armature, std::vector<AnimatedMeshNode> &nodes);
        void updateMatricies();

        std::shared_ptr<MeshArmature> meshArmature;
        std::vector<BoneBinding> bindings;
        std::string ownerName;
    };
};