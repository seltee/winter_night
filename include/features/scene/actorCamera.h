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
        ActorCamera(Renderer *renderer, std::shared_ptr<Camera> camera);

        void update(float delta) override;

        void updateClippingPlanes();

        void updateRenderFlag(ActorCamera *camera) override;

        bool checkFrustrum(const Vector3 &point, float radius)
        {
            return distance(leftClippingPlane, point) > -radius &&
                   distance(rightClippingPlane, point) > -radius &&
                   distance(topClippingPlane, point) > -radius &&
                   distance(bottomClippingPlane, point) > -radius &&
                   distance(nearClippingPlane, point) > -radius &&
                   distance(farClippingPlane, point) > -radius;
        }

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
        Plane leftClippingPlane;
        Plane rightClippingPlane;
        Plane topClippingPlane;
        Plane bottomClippingPlane;
        Plane nearClippingPlane;
        Plane farClippingPlane;
    };
};
