#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/pipelines/vulkanDescriptorSetLayout.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanObjectBuffers.h"
#include <memory>

#include "core/core.h"
#include "core/api.h"

namespace wne
{
    class VulkanShader;
    class VulkanDescriptorLayout;
    class VulkanRenderPass;
    class VulkanDevice;

    class WNE_API VulkanPipelineTextured : public VulkanPipeline
    {
    public:
        VulkanPipelineTextured(VulkanDevice *vulkanDevice);
        ~VulkanPipelineTextured();
        VkPipeline getGraphicsPipeline() override final;
        VkPipelineLayout getPipelineLayout() override final;

        bool setupParametred(VulkanRenderPass *renderPass, bool enableLights, ColorBlending blending, uint64 MSAASmapleCountBit);
        bool setupDepth(VulkanRenderPass *depthPass, uint64 MSAASmapleCountBit);
        bool setupMaskedDepth(VulkanRenderPass *depthPass, uint64 MSAASmapleCountBit);
        bool setupDepthShadow(VulkanRenderPass *depthPass, bool isDoubleSided);
        bool setupMaskedDepthShadow(VulkanRenderPass *depthPass, bool isDoubleSided);
        bool setupAtmosphere(VulkanRenderPass *renderPass, uint64 MSAASmapleCountBit);

        VulkanDescriptorSetLayout *getDescriptorSetLayoutPipeline() override final;
        VulkanDescriptorSetLayout *getDescriptorSetLayoutSampler() override final;

    protected:
        bool buildShaderColor();
        bool buildShaderColorNoLights();
        bool buildShaderDepth();
        bool buildShaderMaskedDepth();
        bool buildShaderAtmosphere();
        bool buildPipeline(
            uint32 stageAmount,
            bool enableColorBlending,
            bool enableDepthWrite,
            bool enableDepthTest,
            bool enableSampler,
            bool faceCooling,
            bool reverseFaceCooling,
            bool opEqual,
            uint64 MSAASmapleCountBit,
            ColorBlending blending,
            VulkanRenderPass *renderPass);

        std::unique_ptr<VulkanDescriptorSetLayout> descriptorSetLayoutPipeline;
        std::unique_ptr<VulkanDescriptorSetLayout> descriptorSetLayoutSampler;

        std::unique_ptr<VulkanShader> shader;
        VkPipelineLayout pipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;
    };
};
