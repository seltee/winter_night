#pragma once
#include "camera.h"
#include "core/core.h"
#include "core/math.h"
#include "features/os/window.h"
#include "core/api.h"
#include <memory>

namespace wne
{
    class WNE_API CameraPerspective : public Camera
    {
    public:
        CameraPerspective();
        CameraPerspective(std::shared_ptr<Window> window);

        static std::shared_ptr<CameraPerspective> create();
        static std::shared_ptr<CameraPerspective> create(std::shared_ptr<Window> window);

        // will automatically update size of orthographics projection upon update
        void setRelatedWindow(std::shared_ptr<Window> window);

        // updates if window is set
        void update() override final;

        void updateMatrix();

        inline void setNearPlane(float newDistance)
        {
            nearPlane = newDistance;
            updateMatrix();
        }

        inline float getNearPlane()
        {
            return nearPlane;
        }

        inline void setFarPlane(float newDistance)
        {
            farPlane = newDistance;
            updateMatrix();
        }

        inline float getFarPlane()
        {
            return farPlane;
        }

        inline void setFov(float newFov)
        {
            fov = newFov;
            updateMatrix();
        }

        inline float getFov()
        {
            return fov;
        }

        inline void setAspectRatio(float newAspectRatio)
        {
            if (!window)
            {
                aspectRatio = newAspectRatio;
                updateMatrix();
            }
        }

        inline float getAspectRatio()
        {
            return aspectRatio;
        }

    protected:
        int32 width = 0, height = 0;
        std::shared_ptr<Window> window;

        float fov = PI / 2.0f;
        float aspectRatio = 1.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
    };
};
