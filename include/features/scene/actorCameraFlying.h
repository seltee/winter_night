#pragma once
#include "core/api.h"
#include "features/scene/actorCamera.h"
#include <memory>

namespace wne
{
    class Window;
    class WindowEvents;
    class Renderer;

    class WNE_API ActorCameraFlying : public ActorCamera
    {
    public:
        ActorCameraFlying(Renderer *renderer, std::shared_ptr<Camera> camera, std::shared_ptr<Window> eventWindow);
        ~ActorCameraFlying();

        void update(float delta) override;

        inline float getSpeed()
        {
            return speed;
        }
        inline void setSpeed(float speed)
        {
            this->speed = speed;
        }

    protected:
        void addCameraRotation(float addYaw, float pitch);

        float moveForward = 0, moveUp = 0, moveRight = 0;
        float rotateYaw = 0, rotatePitch = 0;
        float speed = 40.0f;

        float yaw = 0.0f;
        float pitch = 0.0f;
        
        std::shared_ptr<wne::WindowEvents> eventsSubscription;
    };
};
