#pragma once
#include <memory>
#include "features/data/image.h"
#include "features/renderer/texture.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanImageView.h"

#include "core/core.h"

namespace wne
{
    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture(VulkanUtils *vulkanUtils);
        ~VulkanTexture();
        bool setup(void *data, uint32 width, uint32 height);

        static std::shared_ptr<VulkanTexture> create(std::shared_ptr<Image> image, VulkanUtils *vulkanUtils);

        void bind() override final;

    protected:
        VkBuffer stagingBuffer = nullptr;
        VkDeviceMemory stagingBufferMemory = nullptr;
        VkImage textureImage = nullptr;
        VkDeviceMemory textureImageMemory = nullptr;
        VkSampler textureSampler = nullptr;
        VulkanUtils *vulkanUtils = nullptr;
        std::unique_ptr<VulkanImageView> vulkanImageView;
    };
};