#pragma once
#include "features/renderer/renderer.h"
#include "features/renderer/vulkan/vulkanInstance.h"
#include "core/core.h"

namespace WNE
{
    class RendererVulkanNT : public Renderer
    {
    public:
        void render() override final;
        void changeWindowSize(uint32 width, uint32 height) override final;
        ModelRender *createFromModel(Model *model) override final;

        void setHWND(void *hWnd);
        bool setup(uint32 width, uint32 height) override final;

    protected:
        void *hWnd = nullptr;
        VulkanInstance *instance = nullptr;
    };
}
