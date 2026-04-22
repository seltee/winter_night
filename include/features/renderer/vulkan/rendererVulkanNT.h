#pragma once
#include "features/renderer/renderer.h"
#include "features/renderer/vulkan/vulkanInstance.h"

namespace WNE
{
    class RendererVulkanNT : public Renderer
    {
    public:
        void render() override final;
        void changeWindowSize(int width, int height) override final;

        void setHWND(void *hWnd);
        bool setup(int width, int height) override final;

    protected:
        void *hWnd = nullptr;
        VulkanInstance *instance = nullptr;
    };
}