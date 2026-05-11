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
        alignas(16) Matrix4x4 mvp;
        alignas(16) Matrix4x4 mNormal;
    };

    struct VulkanMaterialDescription;
    class WNE_API VulkanMaterial : public Material
    {
    public:
        VulkanMaterial(VulkanUtils *vulkanUtils);

        static std::shared_ptr<Material> createFlat(VulkanUtils *vulkanUtils, std::shared_ptr<Texture> texture);

        void bind(const Matrix4x4 &mMVP, const Matrix3x3 &mNormal, ModelDataType dataType) override final;

        virtual void selectPipeline(ModelDataType dataType);
        virtual void selectDescriptor(ModelDataType dataType);
        virtual void setPCData(const Matrix4x4 &mMVP, const Matrix3x3 &mNormal);

    protected:
        VulkanUtils *vulkanUtils = nullptr;
    };
};
