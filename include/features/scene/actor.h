#pragma once
#include "core/math.h"
#include "core/api.h"
#include "features/scene/object.h"

namespace wne
{
    class Renderer;
    class Scene;

    class WNE_API Actor : public Object
    {
    public:
        enum RenderPass
        {
            Main,
            Blended
        };

        Actor();

        virtual void update(float delta);

        // build depth buffer for light's shadow
        virtual void renderDepthShadow(Renderer *renderer);

        // build depth buffer
        virtual void renderDepth(Renderer *renderer);

        // render equal to depth buffer
        virtual void renderColor(Renderer *renderer);

        virtual const Matrix4x4 &getModelMatrix();
        virtual const Matrix3x3 &getNormalMatrix();
        virtual Matrix4x4 getInvModelMatrix();
        virtual RenderPass getRenderPass();

        void setScene(Scene *scene);

        virtual void eventSetScene(Scene *oldScene, Scene *newScene);
        virtual void eventUpdate(float delta);
        
        inline void setPosition(float x, float y, float z)
        {
            isDirtyFlag = true;
            position = Vector3(x, y, z);
        }

        inline void setPosition(const Vector3 &v)
        {
            isDirtyFlag = true;
            position = v;
        }

        inline const Vector3 &getPosition()
        {
            return position;
        }

        inline void setRotation(float eulerX, float eulerY, float eulerZ)
        {
            isDirtyFlag = true;
            rotation = Quat(eulerX, eulerY, eulerZ);
        }

        inline void setRotation(const Quat &quat)
        {
            isDirtyFlag = true;
            rotation = quat;
        }

        inline const Quat &getRotation()
        {
            return rotation;
        }

        inline void setScale(float x, float y, float z)
        {
            isDirtyFlag = true;
            scale = Vector3(x, y, z);
        }

        inline void setScale(const Vector3 &v)
        {
            isDirtyFlag = true;
            scale = v;
        }

        inline const Vector3 &getScale()
        {
            return scale;
        }

        inline bool hasShadow()
        {
            return isShadowEnabled;
        }

        inline void setShadow(bool isEnabled)
        {
            isShadowEnabled = isEnabled;
        }

    protected:
        bool isDirtyFlag = true;
        bool isDirtyNormalsFlag = true;
        bool isShadowEnabled = true;

        Quat rotation = Quat();
        Vector3 position = Vector3(0, 0, 0);
        Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

        Matrix4x4 mModel = Matrix4x4::identity();
        Matrix3x3 mNormal = Matrix3x3::identity();

        Scene *currentScene = nullptr;
    };
};
