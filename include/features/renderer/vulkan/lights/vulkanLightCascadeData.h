#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanFrameBuffer.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/data/model.h"
#include <memory>

namespace wne
{
    class VulkanUitls;

    struct CascadeFrame
    {
        std::unique_ptr<VulkanDepthBuffer> vulkanDepthBuffer;
        std::unique_ptr<VulkanFrameBuffer> vulkanFrameBuffer;

        VkBuffer objectBufferMVP;
        VkDeviceMemory objectBufferMVPMemory;
        Matrix4x4 *objectBufferMVPMapped;

        VkDescriptorSet descriptorSetTextured;
    };

    class VulkanLightCascadeData
    {
    public:
        VulkanLightCascadeData(VulkanUtils *vulkanUtils);
        ~VulkanLightCascadeData();
        bool setup(uint32 resolution);
        void updateObjectData(uint32 objectId, const Matrix4x4 &mMVP) noexcept;

        VulkanFrameBuffer *getFrameBuffer(VulkanRenderPass *depthPass);

        VkDescriptorSet getDescriptorSet(ModelDataType dataType, VkDescriptorSetLayout layoutPipeline);

        inline VulkanDepthBuffer *getDepthBuffer()
        {
            return frames[vulkanUtils->getCurrentFrame()].vulkanDepthBuffer.get();
        }

        inline VulkanFrameBuffer *getFrameBuffer()
        {
            return frames[vulkanUtils->getCurrentFrame()].vulkanFrameBuffer.get();
        }

    protected:
        VulkanUtils *vulkanUtils = nullptr;
        std::vector<CascadeFrame> frames;
    };
};