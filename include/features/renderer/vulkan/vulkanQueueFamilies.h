#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include <vector>
#include <set>
#include <optional>
#include <algorithm>
#include <iostream>
#include <cstdint>

namespace WNE
{
    class VulkanQueueFamilies
    {
    public:
        bool setup(VkPhysicalDevice device, VkSurfaceKHR surface);

        inline bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }

        inline std::optional<uint32_t> getGraphicsFamily()
        {
            return graphicsFamily;
        }

        inline std::optional<uint32_t> getPresentFamily()
        {
            return presentFamily;
        }

    protected:
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
    };
}