#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/pipelines/vulkanDescriptorSetLayout.h"
#include <memory>

#include "core/core.h"
#include "core/api.h"

namespace wne
{
    class VulkanShader;
    class VulkanDescriptorLayout;
    class VulkanRenderPass;
    class VulkanDevice;
    class VulkanUtils;

    class WNE_API VulkanPipelineUniversal : public VulkanPipeline
    {
    public:
        struct Options
        {
            uint64 VkMSAASampleCountBit;
            ColorBlending blendingMode;

            // enable bone animations
            bool enableBones;

            // shader will calculate lightning if enabled
            bool enableLightning;

            // pixels will be disscarded by alpha treeshold
            bool enableMasked;

            // Main color pass otherwise it's depth
            bool isMainColorPass;

            // ignore z buffer and always draw
            bool ignoreDepth;
        };

        VulkanPipelineUniversal(VulkanUtils *vulkanUtils);
        ~VulkanPipelineUniversal();

        bool setup(VulkanRenderPass *renderPass, const Options &options);
        bool buildShader(const Options &options);

        VkPipeline getGraphicsPipeline() override final;
        VkPipelineLayout getPipelineLayout() override final;

    protected:
        bool buildPipeline(
            uint32 stageAmount,
            bool enableColorWriting,
            bool enableDepthWrite,
            bool enableDepthTest,
            bool enableSampler,
            bool faceCooling,
            bool reverseFaceCooling,
            bool opEqual,
            uint64 MSAASmapleCountBit,
            ColorBlending blending,
            VulkanRenderPass *renderPass);

        std::unique_ptr<VulkanShader> shader;
        VkPipelineLayout pipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;

        VulkanUtils *vulkanUtils = nullptr;
    };
};
