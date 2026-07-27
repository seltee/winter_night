#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "features/renderer/vulkan/pipelines/vulkanPipelineUniversal.h"
#include "features/renderer/materialFlat.h"
#include "core/api.h"

namespace wne
{
    class VulkanLightCascadeData;

    class WNE_API VulkanMaterialFlat : public MaterialFlat, VulkanMaterial
    {
    public:
        VulkanMaterialFlat(VulkanUtils *vulkanUtils);
        ~VulkanMaterialFlat();

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
            ModelDataType dataType) override final;

        void selectPipelineDepth(ModelDataType dataType) override final;
        void selectPipelineColor(ModelDataType dataType, const MeshArmature *meshArmature) override final;
        void selectPipelineShadowDepth(ModelDataType dataType, bool isDoubleSided) override final;
        void selectDescriptorColor(ModelDataType dataType) override final;
        void selectDescriptorDepth(ModelDataType dataType) override final;
        void selectDescriptorDepthShadow(ModelDataType dataType, VulkanLightCascadeData *cascadeData) override final;
        void setPCData(uint64 objectId, const AffectingLights &lights, const Material::UVData &uvData, const MaterialBoneData &materialBoneData) override final;
        void rebuild() override final;

        void setAlbedoTexture(std::shared_ptr<Texture> albedoTexture) override final;
        std::shared_ptr<Texture> getAlbedoTexture() override final;

    protected:
         void resetPipeline() override final;

         std::shared_ptr<Texture> albedoTexture;
         VkDescriptorSet descriptorSet = nullptr;

         VkDescriptorSet getDescriptorSetFlatTextured();
         void nullifyPipelines();
         void buildColorPipeline(bool enableBones);

         bool isPipelineDirty = true;
         std::unique_ptr<VulkanPipelineUniversal> colorPipeline;
         std::unique_ptr<VulkanPipelineUniversal> colorPipelineWithBones;

         uint64 currentImageLayout = 0;
         VkImageView currentImageView = nullptr;
    };
};