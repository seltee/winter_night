#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/data/model.h"
#include <memory>

namespace wne
{
    class VulkanUitls;
    class VulkanLightCascadeData
    {
    public:
        VulkanLightCascadeData(VulkanUtils *vulkanUtils);
        ~VulkanLightCascadeData();
        bool setup(uint32 resolution);
        void updateObjectData(uint32 objectId, const Matrix4x4 &mMVP) noexcept;

        VulkanFrameBuffer *getFrameBuffer(VulkanRenderPass *depthPass);

        VkDescriptorSet getDescriptorSet(
            ModelDataType dataType,
            VkDescriptorSetLayout layoutPipeline,
            VkDescriptorSetLayout layoutSampler);

        inline VulkanDepthBuffer *getDepthBuffer()
        {
            return vulkanDepthBuffer.get();
        }

        inline VulkanFrameBuffer *getFrameBuffer()
        {
            return vulkanFrameBuffer.get();
        }

    protected:
        VulkanUtils *vulkanUtils = nullptr;

        // used to render shadow buffer
        VkBuffer objectBufferMVP = nullptr;
        VkDeviceMemory objectBufferMVPMemory = nullptr;
        Matrix4x4 *objectBufferMVPMapped = nullptr;

        VkDescriptorSet descriptorSetTextured = nullptr;

        std::unique_ptr<VulkanDepthBuffer> vulkanDepthBuffer;
        std::unique_ptr<VulkanFrameBuffer> vulkanFrameBuffer;
    };
};