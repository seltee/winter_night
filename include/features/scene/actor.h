#pragma once
#include "core/math.h"
#include "core/api.h"
#include "features/scene/object.h"
#include "features/scene/positionable.h"

namespace wne
{
    class Renderer;
    class Scene;

    class WNE_API Actor : public Object, public Positionable
    {
    public:
        enum RenderPass
        {
            Main,
            Blended
        };

        Actor(Renderer *renderer);

        virtual void update(float delta);

        // build depth buffer for light's shadow
        virtual void renderDepthShadow();

        // build depth buffer
        virtual void renderDepth();

        // render equal to depth buffer
        virtual void renderColor();

        void modelMatrixChanged() override;
        virtual const Matrix3x3 &getNormalMatrix();
        virtual RenderPass getRenderPass();

        void setScene(Scene *scene);

        virtual void eventCreated();
        virtual void eventSetScene(Scene *oldScene, Scene *newScene);
        virtual void eventUpdate(float delta);
        virtual void eventDestroyed();

        inline bool hasShadow()
        {
            return isShadowEnabled;
        }

        inline void setShadow(bool isEnabled)
        {
            isShadowEnabled = isEnabled;
        }

        inline void destroy()
        {
            isDestroyedFlag = true;
        }

        inline bool isDestroyed()
        {
            return isDestroyedFlag;
        }

    protected:
        bool isDirtyNormalsFlag = true;
        bool isShadowEnabled = true;
        bool isDestroyedFlag = false;

        Matrix3x3 mNormal = Matrix3x3::identity();

        Scene *currentScene = nullptr;
        Renderer *renderer = nullptr;
    };
};
