#pragma once
#include "features/scene/object.h"
#include "features/scene/actor.h"
#include "features/camera/camera.h"
#include "core/api.h"
#include "core/math.h"
#include "core/api.h"
#include <memory>

namespace wne
{
    class WNE_API ActorCamera : public Actor
    {
    public:
        ActorCamera(std::shared_ptr<Camera> camera);
        static std::shared_ptr<ActorCamera> create(std::shared_ptr<Camera> camera);

        void update(float delta) override final;

        const Matrix4x4 &getProjectionMatrix()
        {
            return camera->getProjectionMatrix();
        }
        const Matrix4x4 getViewMatrix()
        {
            return getInvModelMatrix();
        }

    protected:
        std::shared_ptr<Camera> camera;
    };
};
