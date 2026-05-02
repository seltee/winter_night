#pragma once
#include "camera.h"
#include "core/core.h"
#include "features/os/window.h"
#include <memory>

namespace wne
{
    class CameraOrtho : public Camera
    {
    public:
        CameraOrtho();
        CameraOrtho(uint32 width, uint32 height);
        CameraOrtho(std::shared_ptr<Window> window);

        // will automatically update size of orthographics projection upon update
        void setRelatedWindow(std::shared_ptr<Window> window);

        // ignored if window is set
        void setSize(uint32 width, uint32 height);

        // updates if window is set
        void update();

        void updateMatrix();

        inline void setNearDistance(float newDistance)
        {
            near = newDistance;
            updateMatrix();
        }

        inline float getNearDistance()
        {
            return near;
        }

        inline void setFarDistance(float newDistance)
        {
            far = newDistance;
            updateMatrix();
        }

        inline float getFarDistance()
        {
            return far;
        }

    protected:
        uint32 width = 0, height = 0;
        std::shared_ptr<Window> window;

        float near = 0.0f;
        float far = 10.0f;
    };
};
