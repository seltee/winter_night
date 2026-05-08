#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "core/math.h"
#include "core/api.h"
#include <vector>

namespace wne
{
    class VulkanUtils;

    class WNE_API VulkanDepthBuffer
    {
    public:
        VulkanDepthBuffer(VulkanUtils *vulkanUtils);
        ~VulkanDepthBuffer();
        bool setup(uint16 width, uint16 height);

        inline VulkanFormat getFormat()
        {
            return format;
        }

        inline VkImage getDepthImage()
        {
            return depthImage;
        }

        inline VkImageView getDepthImageView()
        {
            return depthImageView;
        }

    protected:
        VulkanFormat format;

        VulkanUtils *vulkanUtils = nullptr;

        VkImage depthImage = nullptr;
        VkDeviceMemory depthImageMemory = nullptr;
        VkImageView depthImageView = nullptr;

        VulkanFormat findDepthFormat();
        bool hasStencilComponent(VulkanFormat format);
    };
};
