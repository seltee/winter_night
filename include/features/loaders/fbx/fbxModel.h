#pragma once
#include "fbxNode.h"
#include "fbxGeometry.h"
#include "features/data/model.h"
#include <memory>

namespace wne
{
    class FBXModel
    {
    public:
        FBXModel(FBXNode &node);
        std::shared_ptr<Model> getAsModel();

        inline uint64 getId()
        {
            return id;
        }

        inline void setParent(FBXModel *parent)
        {
            this->parent = parent;
        }

        inline void setGeometry(FBXGeometry *geometry)
        {
            this->geometry = geometry;
        }

        inline FBXGeometry *getGeometry()
        {
            return geometry;
        }

        inline const char *getName()
        {
            return name;
        }

        inline Vector3 getPosition()
        {
            return position;
        }

        inline Vector3 getRotation()
        {
            return rotation;
        }

        inline Vector3 getScale()
        {
            return scale;
        }

    protected:
        uint64 id;
        const char *name = nullptr;
        bool isDirtyFlag = true;

        Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
        Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
        Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);
        FBXGeometry *geometry = nullptr;
        FBXModel *parent = nullptr;
    };
};
