#pragma once
#include "features/renderer/material.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanSampler.h"
#include "features/renderer/vulkan/vulkanDescriptorSet.h"

namespace wne
{
    struct PushConstantObject
    {
        alignas(16) Matrix4x4 mvp;
    };

    struct VulkanMaterialDescription;
    class VulkanMaterial : public Material
    {
    public:
        VulkanMaterial(VulkanUtils *vulkanUtils);
        ~VulkanMaterial();

        static std::shared_ptr<Material> createFlat(VulkanUtils *vulkanUtils, std::shared_ptr<Texture> texture);

        void rebuild() override final;
        void bind(const Matrix4x4 &mMVP) override final;

        inline void setAlbedo(std::shared_ptr<Texture> texture)
        {
            isDirty = true;
            albedoTexture = std::move(texture);
        }

    protected:
        bool isDirty = true;
        std::shared_ptr<Texture> albedoTexture;
        std::vector<std::unique_ptr<VulkanDescriptorSet>> descriptorSet;

        VulkanUtils *vulkanUtils = nullptr;
        VkBuffer uniformBuffer = nullptr;
        VkDeviceMemory uniformBuffersMemory = nullptr;
        VkDescriptorSet vkDescriptorSet = nullptr;
        void *uniformBuffersMapped = nullptr;

        VulkanMaterialDescription *description = nullptr;
    };
};
