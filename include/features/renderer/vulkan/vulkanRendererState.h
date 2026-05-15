#pragma once
#include "features/renderer/rendererState.h"
#include "features/renderer/vulkan/lights/vulkanLightCascadeData.h"

namespace wne
{
    class WNE_API VulkanRendererState : public RendererState
    {
    public:
        inline void setVulkanLightCascadeData(VulkanLightCascadeData *vulkanLightCascadeData)
        {
            this->vulkanLightCascadeData = vulkanLightCascadeData;
        }

        inline VulkanLightCascadeData *getVulkanLightCascadeData()
        {
            return vulkanLightCascadeData;
        }

    protected:
        VulkanLightCascadeData *vulkanLightCascadeData = nullptr;
    };
};
