#pragma once
#include "features/scene/light.h"

namespace wne
{
    class VulkanUtils;
    class VulkanLightSpot : public Light
    {
    public:
        VulkanLightSpot(VulkanUtils *vulkanUtils);
        ~VulkanLightSpot();

    protected:
        VulkanUtils *vulkanUtils = nullptr;
    };
};
