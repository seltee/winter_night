#pragma once
#include "features/renderer/material.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanSampler.h"
#include "core/api.h"

namespace wne
{
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

        void bindDepth(uint64 objectId, const Matrix4x4 &mMVP, const Matrix3x3 &mNormal, ModelDataType dataType) override final;
        void bindColor(uint64 objectId, const AffectingLights &lights, const Matrix4x4 &mMVP, const Matrix3x3 &mNormal, ModelDataType dataType) override final;

        virtual void selectPipelineDepth(ModelDataType dataType);
        virtual void selectPipelineColor(ModelDataType dataType);

        virtual void selectDescriptor(ModelDataType dataType);
        virtual void setPCData(uint64 objectId, const AffectingLights &lights);

    protected:
        VulkanUtils *vulkanUtils = nullptr;
    };
};
