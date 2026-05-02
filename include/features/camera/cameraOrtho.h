#pragma once
#include "camera.h"
#include "core/core.h"
#include "features/os/window.h"
#include "core/api.h"
#include <memory>

namespace wne
{
    class WNE_API CameraOrtho : public Camera
    {
    public:
        enum class Base
        {
            None,
            Height,
            Width
        };

        CameraOrtho();
        CameraOrtho(uint32 width, uint32 height);
        CameraOrtho(std::shared_ptr<Window> window);

        static std::shared_ptr<CameraOrtho> create(uint32 width, uint32 height);
        static std::shared_ptr<CameraOrtho> create(std::shared_ptr<Window> window);
        static std::shared_ptr<CameraOrtho> createHeightBased(std::shared_ptr<Window> window, float fixedHeight);
        static std::shared_ptr<CameraOrtho> createWidthBased(std::shared_ptr<Window> window, float fixedWidth);

        // will automatically update size of orthographics projection upon update
        void setRelatedWindow(std::shared_ptr<Window> window);

        // ignored if window is set
        void setSize(uint32 width, uint32 height);

        // updates if window is set
        void update() override final;

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

        // works only if you use window
        inline void setBase(Base base, float baseValue = 0.0f)
        {
            this->base = base;
            this->baseValue = baseValue;
            update();
        }

    protected:
        uint32 width = 0, height = 0;
        std::shared_ptr<Window> window;

        Base base = Base::None;
        float baseValue = 1.0f;

        float near = 0.0f;
        float far = 10.0f;
    };
};
