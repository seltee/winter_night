#pragma once
#include "core/api.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include <vector>

namespace WNE
{
    class WNE_API VulkanDeviceExtensions
    {
    public:
        VulkanDeviceExtensions(VkPhysicalDevice device);
        bool hasExtension(const char *extName);

    protected:
        std::vector<const char *> exts;
    };
}