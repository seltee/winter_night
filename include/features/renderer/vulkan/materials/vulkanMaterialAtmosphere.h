#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "core/api.h"

namespace wne
{
    class VulkanLightCascadeData;

    class WNE_API VulkanMaterialAtmosphere : public VulkanMaterial
    {
    public:
        VulkanMaterialAtmosphere(VulkanUtils *vulkanUtils);
        ~VulkanMaterialAtmosphere();

        void selectPipelineColor(ModelDataType dataType) override final;
        void selectDescriptorColor(ModelDataType dataType) override final;
        void setPCData(uint64 objectId, const AffectingLights &lights, const UVData &uvData) override final;

        inline void setAtmosphereTexture(std::shared_ptr<Texture> texture)
        {
            atmosphereTexture = texture;
        }

    protected:
        std::shared_ptr<Texture> atmosphereTexture;
        VkDescriptorSet descriptorSet = nullptr;

        VkDescriptorSet getDescriptorSetAtmoTexture();
    };
};