#pragma once
#include "features/renderer/material.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanSampler.h"
#include "core/api.h"

namespace wne
{
    class VulkanLightCascadeData;
    struct PushConstantObject
    {
        uint32 objectId;
        AffectingLights lights;
    };

    struct VulkanMaterialDescription;
    class WNE_API VulkanMaterial : public Material
    {
    public:
        VulkanMaterial(VulkanUtils *vulkanUtils);

        static std::shared_ptr<Material> createFlat(VulkanUtils *vulkanUtils, std::shared_ptr<Texture> texture);

        void bindDepthShadow(uint64 objectId, Renderer *renderer, const Matrix4x4 &mMVP, const Matrix3x3 &mNormal, ModelDataType dataType) override final;
        void bindDepth(uint64 objectId, const Matrix4x4 &mMVP, const Matrix4x4 &mModel, const Matrix3x3 &mNormal, ModelDataType dataType) override final;
        void bindColor(uint64 objectId, const AffectingLights &lights, const Matrix4x4 &mMVP, const Matrix4x4 &mModel, const Matrix3x3 &mNormal, ModelDataType dataType) override final;

        virtual void selectPipelineDepth(ModelDataType dataType);
        virtual void selectPipelineColor(ModelDataType dataType);

        virtual void selectDescriptorColor(ModelDataType dataType);
        virtual void selectDescriptorDepth(ModelDataType dataType);
        virtual void selectDescriptorDepthShadow(ModelDataType dataType, VulkanLightCascadeData *cascadeData);
        virtual void setPCData(uint64 objectId, const AffectingLights &lights);

    protected:
        VulkanUtils *vulkanUtils = nullptr;
    };
};
