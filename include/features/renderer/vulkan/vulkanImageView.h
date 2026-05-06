#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"

namespace wne
{
    class VulkanDevice;

    class VulkanImageView
    {
    public:
        VulkanImageView(VulkanDevice *vulkanDevice);
        ~VulkanImageView();
        bool setup(VkImage image, uint64 format);

        inline VkImageView getImageView()
        {
            return imageView;
        }

    protected:
        VulkanDevice *vulkanDevice = nullptr;
        VkImageView imageView = nullptr;
    };
};
