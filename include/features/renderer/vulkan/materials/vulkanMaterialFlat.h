#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "core/api.h"

namespace wne
{
    class VulkanLightCascadeData;

    class WNE_API VulkanMaterialFlat : public VulkanMaterial
    {
    public:
        VulkanMaterialFlat(VulkanUtils *vulkanUtils);
        ~VulkanMaterialFlat();

        void selectPipelineDepth(ModelDataType dataType) override final;
        void selectPipelineColor(ModelDataType dataType) override final;
        void selectPipelineColorNoLights(ModelDataType dataType) override final;
        void selectPipelineShadowDepth(ModelDataType dataType) override final;
        void selectDescriptorColor(ModelDataType dataType) override final;
        void selectDescriptorDepth(ModelDataType dataType) override final;
        void selectDescriptorDepthShadow(ModelDataType dataType, VulkanLightCascadeData *cascadeData) override final;
        void setPCData(uint64 objectId, const AffectingLights &lights) override final;
        void rebuild() override final;

        inline void setAlbedo(std::shared_ptr<Texture> texture)
        {
            albedoTexture = texture;
        }

    protected:
        std::shared_ptr<Texture> albedoTexture;
        VkDescriptorSet descriptorSet = nullptr;

        VkDescriptorSet getDescriptorSetFlatTextured();
    };
};