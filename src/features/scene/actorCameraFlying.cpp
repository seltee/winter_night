#include "features/scene/actorCameraFlying.h"
#include "features/os/windowEvents.h"
#include "features/os/window.h"
#include "core/math.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace wne;

ActorCameraFlying::ActorCameraFlying(Renderer *renderer, std::shared_ptr<Camera> camera, std::shared_ptr<Window> eventWindow) : ActorCamera(renderer, camera)
{
    eventsSubscription = eventWindow->subscribe();
}

ActorCameraFlying::~ActorCameraFlying()
{
}

void ActorCameraFlying::update(float delta)
{
    if (camera)
        camera->update();

    if (!eventsSubscription)
        return;

    WindowEvents::WindowEvent event;
    while (eventsSubscription->getEvent(&event))
    {
        if (event.type == WindowEvents::WindowEventType::KEY_DOWN)
        {
            if (event.key.scancode == 87)
                moveForward = true;
            else if (event.key.scancode == 83)
                moveBackward = true;
            else if (event.key.scancode == 65)
                moveLeft = true;
            else if (event.key.scancode == 68)
                moveRight = true;
            else if (event.key.scancode == 81)
                moveUp = true;
            else if (event.key.scancode == 69)
                moveDown = true;
        }
        else if (event.type == WindowEvents::WindowEventType::KEY_UP)
        {
            if (event.key.scancode == 87)
                moveForward = false;
            else if (event.key.scancode == 83)
                moveBackward = false;
            else if (event.key.scancode == 65)
                moveLeft = false;
            else if (event.key.scancode == 68)
                moveRight = false;
            else if (event.key.scancode == 81)
                moveUp = false;
            else if (event.key.scancode == 69)
                moveDown = false;
        }
        else if (event.type == WindowEvents::WindowEventType::MOUSE_MOVE)
        {
            yaw += (float)event.mouseMove.shiftX * 0.009f;   // left/right
            pitch += (float)event.mouseMove.shiftY * 0.009f; // up/down

            yaw = wrap2Pi(yaw);

            pitch = std::clamp(pitch, -2.4f, 2.4f);

            Quat qPitch = Quat::aroundWorldX(pitch);
            Quat qYaw = Quat::aroundWorldY(yaw);
            rotation = normalize(qYaw * qPitch);
            isDirtyFlag = true;
        }
        else if (event.type == WindowEvents::WindowEventType::WINDOW_FOCUSED || event.type == WindowEvents::WindowEventType::WINDOW_UNFOCUSED)
        {
            moveForward = false;
            moveBackward = false;
            moveLeft = false;
            moveRight = false;
            moveUp = false;
            moveDown = false;
            isDirtyFlag = true;
        }
    }

    Vector3 forward = rotation * Vector3::forward();
    Vector3 right = rotation * Vector3::right();
    Vector3 up = rotation * Vector3::up();

    if (moveForward)
    {
        position += forward * delta * speed;
        isDirtyFlag = true;
    }
    if (moveBackward)
    {
        position -= forward * delta * speed;
        isDirtyFlag = true;
    }
    if (moveLeft)
    {
        position -= right * delta * speed;
        isDirtyFlag = true;
    }
    if (moveRight)
    {
        position += right * delta * speed;
        isDirtyFlag = true;
    }
    if (moveUp)
    {
        position -= up * delta * speed;
        isDirtyFlag = true;
    }
    if (moveDown)
    {
        position += up * delta * speed;
        isDirtyFlag = true;
    }
    eventUpdate(delta);

    updateClippingPlanes();
}
