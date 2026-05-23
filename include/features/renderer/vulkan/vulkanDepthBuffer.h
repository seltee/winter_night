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

        // width, height - resolution
        // isSampled - will depth be used with sampler in shaders
        bool setup(uint16 width, uint16 height, uint64 sampledCountBit, bool isSampled = false);
        void transitionToDefined();

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

        inline uint16 getWidth()
        {
            return width;
        }

        inline uint16 getHeight()
        {
            return height;
        }

    protected:
        uint16 width = 0, height = 0;
        VulkanFormat format;

        VulkanUtils *vulkanUtils = nullptr;

        VkImage depthImage = nullptr;
        VkDeviceMemory depthImageMemory = nullptr;
        VkImageView depthImageView = nullptr;

        VulkanFormat findDepthFormat(bool isSampled);
        bool hasStencilComponent(VulkanFormat format);
    };
};
