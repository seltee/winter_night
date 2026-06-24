#pragma once
#include "core/math.h"
#include "core/api.h"
#include <vector>
#include <memory>
#include <iostream>

namespace wne
{
    class WNE_API Positionable
    {
    public:
        virtual const Matrix4x4 &getModelMatrix();
        virtual Matrix4x4 getInvModelMatrix();
        virtual void modelMatrixChanged();

        inline Vector3 getForwardVector()
        {
            Quat p(0.0f, 0.0f, -1.0f, 0.0f);
            Quat qInv(-rotation.x, -rotation.y, -rotation.z, rotation.w);
            Quat result = rotation * p * qInv;
            return normalize(Vector3(result.x, result.y, result.z));
        }

        inline Vector3 getRightVector()
        {
            Quat p(1.0f, 0.0f, 0.0f, 0.0f);
            Quat qInv(-rotation.x, -rotation.y, -rotation.z, rotation.w);
            Quat result = rotation * p * qInv;
            return normalize(Vector3(result.x, result.y, result.z));
        }

        inline Vector3 getUpVector()
        {
            Quat p(0.0f, 1.0f, 0.0f, 0.0f);
            Quat qInv(-rotation.x, -rotation.y, -rotation.z, rotation.w);
            Quat result = rotation * p * qInv;
            return normalize(Vector3(result.x, result.y, result.z));
        }

        inline void setPosition(float x, float y, float z)
        {
            makeDirty();
            position = Vector3(x, y, z);
        }

        inline void setPosition(const Vector3 &v)
        {
            makeDirty();
            position = v;
        }

        inline const Vector3 &getPosition()
        {
            return position;
        }

        inline void setRotation(float eulerX, float eulerY, float eulerZ)
        {
            makeDirty();
            rotation = Quat(eulerX, eulerY, eulerZ);
        }

        inline void setRotation(const Quat &quat)
        {
            makeDirty();
            rotation = quat;
        }

        inline const Quat &getRotation()
        {
            return rotation;
        }

        inline void setScale(float x, float y, float z)
        {
            makeDirty();
            scale = Vector3(x, y, z);
        }

        inline void setScale(const Vector3 &v)
        {
            makeDirty();
            scale = v;
        }

        inline const Vector3 &getScale()
        {
            return scale;
        }

        void makeDirty()
        {
            isDirtyFlag = true;
            for (const auto &child : children)
            {
                child->makeDirty();
            }
        }

        void removeChild(Positionable *child, bool removeParentFromChild = true)
        {
            if (removeParentFromChild)
                child->removeParent(false);
            std::erase_if(children, [&](Positionable *pos)
                          { return pos == child; });
        }

        void removeParent(bool removeChildFromParent = true)
        {
            if (parent)
            {
                if (removeChildFromParent)
                    parent->removeChild(this, false);
                parent = nullptr;
                makeDirty();
            }
        }

        void addChild(Positionable *child, bool setParentToChild = true)
        {
            if (setParentToChild)
                child->setParent(this);
            children.push_back(child);
        }

        void setParent(Positionable *parent, bool setChildToParent = true)
        {
            removeParent();
            if (parent)
            {
                parent->addChild(this, false);
                this->parent = parent;
            }
            makeDirty();
        }

    protected:
        bool isDirtyFlag = true;
        Positionable *parent = nullptr;
        std::vector<Positionable *> children;

        Quat rotation = Quat();
        Vector3 position = Vector3(0, 0, 0);
        Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

        Matrix4x4 mModel = Matrix4x4::identity();
    };
};