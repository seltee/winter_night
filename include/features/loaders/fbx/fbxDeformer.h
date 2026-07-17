#pragma once
#include "core/core.h"
#include "fbxNode.h"
#include <memory>
#include <vector>

namespace wne
{
    class FBXDeformer
    {
    public:
        FBXDeformer(FBXNode &node);

        inline const uint64 getId()
        {
            return id;
        }

        inline const char *getName()
        {
            return name.c_str();
        }

        inline const char *getTypeName()
        {
            return typeName.c_str();
        }

        inline bool isSkin()
        {
            return isSkinFlag;
        }

        inline bool isCluster()
        {
            return isClusterFlag;
        }

        inline void setParent(FBXDeformer *parent)
        {
            this->parent = parent;
        }

        inline FBXDeformer *getParent()
        {
            return parent;
        }

        inline void addChildDeformer(FBXDeformer *child)
        {
            children.push_back(child);
        }

        inline const std::vector<FBXDeformer *> &getChildrenDeformers()
        {
            return children;
        }

    private:
        uint64 id = 0;
        std::string name;
        std::string typeName;

        std::vector<int> indexes;
        std::vector<float> weights;
        Matrix4x4 mInvTransform;

        FBXDeformer *parent = nullptr;
        std::vector<FBXDeformer *> children;

        bool isSkinFlag = false;
        bool isClusterFlag = false;
    };
};
