#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "features/renderer/vulkan/pipelines/vulkanPipelineUniversal.h"
#include "core/api.h"

namespace wne
{
    class VulkanLightCascadeData;

    class WNE_API VulkanMaterialAtmosphere : public Material, VulkanMaterial
    {
    public:
        VulkanMaterialAtmosphere(VulkanUtils *vulkanUtils);
        ~VulkanMaterialAtmosphere();

        void bindDepthShadow(
            uint64 objectId,
            Renderer *renderer,
            const Matrix4x4 &mMVP,
            const Matrix3x3 &mNormal,
            const UVData &uvData,
            const MeshArmature *meshArmature,
            bool isDoubleSided,
            ModelDataType dataType) override final;
        void bindDepth(
            uint64 objectId,
            const Matrix4x4 &mMVP,
            const Matrix4x4 &mModel,
            const Matrix3x3 &mNormal,
            const UVData &uvData,
            const MeshArmature *meshArmature,
            ModelDataType dataType) override final;
        void bindColor(
            uint64 objectId,
            const AffectingLights &lights,
            const Matrix4x4 &mMVP,
            const Matrix4x4 &mModel,
            const Matrix3x3 &mNormal,
            const UVData &uvData,
            const MeshArmature *meshArmature,
            Texture *radianceMap,
            ModelDataType dataType) override final;

        void selectPipelineColor(ModelDataType dataType, const MeshArmature *meshArmature) override final;
        void selectDescriptorColor(ModelDataType dataType, VulkanTexture *radianceMap) override final;
        void setPCData(uint64 objectId, const AffectingLights &lights, const UVData &uvData, const MaterialBoneData &materialBoneData) override final;

        inline void setAtmosphereTexture(std::shared_ptr<Texture> texture)
        {
            atmosphereTexture = texture;
        }

    protected:
        void resetPipeline() override final;
        void buildColorPipeline();
        void nullifyPipelines();

        std::shared_ptr<Texture> atmosphereTexture;
        VkDescriptorSet descriptorSet = nullptr;

        std::unique_ptr<VulkanPipelineUniversal> colorPipeline;
        std::unique_ptr<VulkanDescriptorSets> colorPipelineDescriptorSets;

        VkDescriptorSet getDescriptorSetAtmoTexture();
    };
};