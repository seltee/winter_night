#pragma once
#include "features/scene/light.h"

namespace wne
{
    class VulkanUtils;
    class VulkanLightOmni : public Light
    {
    public:
        VulkanLightOmni(VulkanUtils *vulkanUtils);
        ~VulkanLightOmni();

    protected:
        VulkanUtils *vulkanUtils = nullptr;
    };
};
