#pragma once
#include "features/renderer/material.h"
#include "features/renderer/materialFlat.h"
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
        float normalShadowingFactor;
        float uvShiftX, uvShiftY;
        float uvScaleX, uvScaleY;
        uint enableBones, boneWeightsShift, boneMatrixesShift;
    };

    struct MaterialBoneData
    {
        uint enableBones;
        uint boneWeightsShift;
        uint boneMatrixesShift;
    };

    struct VulkanMaterialDescription;
    class WNE_API VulkanMaterial
    {
    public:
        VulkanMaterial(VulkanUtils *vulkanUtils);

        static std::shared_ptr<MaterialFlat> createFlat(VulkanUtils *vulkanUtils);
        static std::shared_ptr<MaterialFlat> createFlat(VulkanUtils *vulkanUtils, std::shared_ptr<Texture> texture);
        static std::shared_ptr<Material> createAtmosphere(VulkanUtils *vulkanUtils, std::shared_ptr<Texture> texture);

        virtual void selectPipelineDepth(ModelDataType dataType, const MeshArmature *meshArmature);
        virtual void selectPipelineColor(ModelDataType dataType, const MeshArmature *meshArmature);
        virtual void selectPipelineShadowDepth(ModelDataType dataType, bool isDoubleSided);

        virtual void selectDescriptorColor(ModelDataType dataType);
        virtual void selectDescriptorDepth(ModelDataType dataType);
        virtual void selectDescriptorDepthShadow(ModelDataType dataType, VulkanLightCascadeData *cascadeData);
        virtual void setPCData(uint64 objectId, const AffectingLights &lights, const Material::UVData &uvData, const MaterialBoneData &materialBoneData);

        inline static void clearLastDescriptorColorBond()
        {
            lastDescriptorColorBond = nullptr;
        }

        static void resetPipelines();
    protected:
        static void addVulkanMaterial(VulkanMaterial *material);
        static void removeVulkanMaterial(VulkanMaterial *material);
        virtual void resetPipeline();

        static std::vector<VulkanMaterial*> vulkanMaterialList;

        VulkanUtils *vulkanUtils = nullptr;
        static VulkanMaterial *lastDescriptorColorBond;
    };
};
