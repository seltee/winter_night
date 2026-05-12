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
    };

    struct VulkanMaterialDescription;
    class WNE_API VulkanMaterial : public Material
    {
    public:
        VulkanMaterial(VulkanUtils *vulkanUtils);

        static std::shared_ptr<Material> createFlat(VulkanUtils *vulkanUtils, std::shared_ptr<Texture> texture);

        void bind(uint64 objectId, const Matrix4x4 &mMVP, const Matrix3x3 &mNormal, ModelDataType dataType) override final;

        virtual void selectPipeline(ModelDataType dataType);
        virtual void selectDescriptor(ModelDataType dataType);
        virtual void setPCData(uint64 objectId);

    protected:
        VulkanUtils *vulkanUtils = nullptr;
    };
};
