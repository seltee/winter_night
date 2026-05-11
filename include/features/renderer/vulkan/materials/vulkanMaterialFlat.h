#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "core/api.h"

namespace wne
{
    class WNE_API VulkanMaterialFlat : public VulkanMaterial
    {
    public:
        VulkanMaterialFlat(VulkanUtils *vulkanUtils);
        ~VulkanMaterialFlat();

        void selectPipeline(ModelDataType dataType) override final;
        void selectDescriptor(ModelDataType dataType) override final;
        void setPCData(const Matrix4x4 &mMVP, const Matrix3x3 &mNormal) override final;
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