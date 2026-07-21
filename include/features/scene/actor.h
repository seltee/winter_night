#pragma once
#include "core/math.h"
#include "core/api.h"
#include "features/scene/object.h"
#include "features/scene/positionable.h"

namespace wne
{
    class Renderer;
    class Scene;
    class ActorCamera;

    class WNE_API Actor : public Object, public Positionable
    {
    public:
        Actor(Renderer *renderer);

        virtual void update(float delta);

        virtual void updateRenderFlag(ActorCamera *camera);

        // build depth buffer for light's shadow
        virtual void renderDepthShadow(Vector3 &lightPosition);

        // build depth buffer
        virtual void renderDepth();

        // render equal to depth buffer
        virtual void renderColor(bool isBlendingPhase);

        virtual bool isBlendingPassRequired();

        void modelMatrixChanged() override;
        virtual const Matrix3x3 &getNormalMatrix();

        void setScene(Scene *scene);

        virtual void eventCreated();
        virtual void eventSetScene(Scene *oldScene, Scene *newScene);
        virtual void eventUpdate(float delta);
        virtual void eventDestroyed();
        virtual float getBoundingRadius();

        inline bool hasShadow()
        {
            return isShadowEnabled;
        }

        inline void setShadowEnabled(bool state)
        {
            isShadowEnabled = state;
        }

        inline void destroy()
        {
            isDestroyedFlag = true;
        }

        inline bool isDestroyed()
        {
            return isDestroyedFlag;
        }

        inline bool isInRender()
        {
            return isInRenderFlag;
        }

        inline void setVisibility(bool state)
        {
            this->isVisibleflag = state;
        }

        inline bool isVisible()
        {
            return isVisibleflag;
        }

        inline void moveForward(float distance)
        {
            Vector3 forward = rotation * Vector3::forward();
            position += forward * distance;
        }

        inline void moveRight(float distance)
        {
            Vector3 right = rotation * Vector3::right();
            position += right * distance;
        }

        inline void moveUp(float distance)
        {
            Vector3 up = rotation * Vector3::up();
            position += up * distance;
        }

    protected:
        bool isDirtyNormalsFlag = true;
        bool isShadowEnabled = true;
        bool isDestroyedFlag = false;

        // is in frustrum
        bool isInRenderFlag = false;
        bool isVisibleflag = true;
        Matrix3x3 mNormal = Matrix3x3::identity();

        Scene *currentScene = nullptr;
        Renderer *renderer = nullptr;
    };
};
