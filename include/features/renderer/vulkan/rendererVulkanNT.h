#pragma once
#include "features/renderer/renderer.h"
#include "features/renderer/vulkan/vulkanInstance.h"
#include "core/core.h"
#include <memory>

namespace WNE
{
    class RendererVulkanNT : public Renderer
    {
    public:
        static std::unique_ptr<RendererVulkanNT> create(void *hwnd, uint32 w, uint32 h)
        {
            auto instance = std::unique_ptr<RendererVulkanNT>(new RendererVulkanNT());
            if (!instance->setup(hwnd, w, h))
                return nullptr;
            return instance;
        }

        void render() override final;
        void changeWindowSize(uint32 width, uint32 height) override final;
        ModelRender *createFromModel(Model *model) override final;

    protected:
        RendererVulkanNT() = default;

        bool setup(void *hWnd, uint32 width, uint32 height);

        void *hWnd = nullptr;
        std::unique_ptr<VulkanInstance> instance;
    };
}
