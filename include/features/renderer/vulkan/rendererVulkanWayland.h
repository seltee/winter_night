#pragma once
#include "features/renderer/renderer.h"
#include "features/renderer/vulkan/rendererVulkan.h"
#include "features/renderer/vulkan/vulkanInstance.h"
#include "features/renderer/vulkan/vulkanText.h"
#include "core/core.h"
#include <memory>

namespace wne
{
    class WNE_API RendererVulkanWayland : public RendererVulkan
    {
    public:
        static std::unique_ptr<RendererVulkanWayland> create(void *wlDisplay, void *wlSurface, int32 width, int32 height);

    protected:
        bool setup(void *wlDisplay, void *wlSurface, int32 width, int32 height);
        void *wlDisplay = nullptr;
        void *wlSurface = nullptr;
    };
}
