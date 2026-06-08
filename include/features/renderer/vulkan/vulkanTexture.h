#pragma once
#include <memory>
#include "features/data/image.h"
#include "features/renderer/texture.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanImageView.h"

#include "core/core.h"
#include <iostream>

namespace wne
{
    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture(VulkanUtils *vulkanUtils);
        ~VulkanTexture();
        bool setup(void *pixels, uint32 width, uint32 height) override final;
        bool setup(VkImageView vkImageView);

        static std::shared_ptr<VulkanTexture> create(std::shared_ptr<Image> image, VulkanUtils *vulkanUtils);

        void bind() override final;

        inline uint64 getImageLayout()
        {
            return imageLayout;
        }

        inline VulkanImageView *getImageView()
        {
            return vulkanImageView.get();
        }

    protected:
        VkBuffer stagingBuffer = nullptr;
        VkDeviceMemory stagingBufferMemory = nullptr;
        VkImage textureImage = nullptr;
        VkDeviceMemory textureImageMemory = nullptr;
        VulkanUtils *vulkanUtils = nullptr;
        std::unique_ptr<VulkanImageView> vulkanImageView;
        uint64 imageLayout = 0;
    };
};