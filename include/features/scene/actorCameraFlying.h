#pragma once
#include "core/api.h"
#include "features/scene/actorCamera.h"
#include <memory>

namespace wne
{
    class Window;
    class WindowEvents;

    class WNE_API ActorCameraFlying : public ActorCamera
    {
    public:
        ActorCameraFlying(std::shared_ptr<Camera> camera, std::shared_ptr<Window> eventWindow);
        ~ActorCameraFlying();

        void update(float delta) override;

        static std::shared_ptr<ActorCameraFlying> create(std::shared_ptr<Camera> camera, std::shared_ptr<Window> eventWindow);

        inline float getSpeed()
        {
            return speed;
        }
        inline void setSpeed(float speed)
        {
            this->speed = speed;
        }

    protected:
        std::shared_ptr<wne::WindowEvents> eventsSubscription;

        bool moveForward = false, moveBackward = false, moveUp = false, moveDown = false, moveLeft = false, moveRight = false;
        float speed = 40.0f;

        float yaw = 0.0f; 
        float pitch = 0.0f;
    };
};
