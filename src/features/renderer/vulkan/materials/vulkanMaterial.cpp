#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "features/renderer/vulkan/materials/vulkanMaterialFlat.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanRendererState.h"
#include "features/renderer/vulkan/rendererVulkanNT.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <array>
#include <iostream>

using namespace wne;

VulkanMaterial::VulkanMaterial(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

std::shared_ptr<Material> VulkanMaterial::createFlat(VulkanUtils *vulkanUtils, std::shared_ptr<Texture> texture)
{
    std::shared_ptr<VulkanMaterialFlat> material = std::make_shared<VulkanMaterialFlat>(vulkanUtils);
    material->setAlbedo(texture);
    material->rebuild();
    return material;
}

void VulkanMaterial::bindDepthShadow(uint64 objectId, Renderer *renderer, const Matrix4x4 &mMVP, const Matrix3x3 &mNormal, ModelDataType dataType)
{
    auto state = (VulkanRendererState *)renderer->getState();
    VulkanLightCascadeData *cascadeData = state->getVulkanLightCascadeData();
    if (dataType == ModelDataType::Unknown || !cascadeData)
        return;

    AffectingLights lights{};
    selectPipelineShadowDepth(dataType);
    selectDescriptorDepthShadow(dataType, cascadeData);
    cascadeData->updateObjectData(objectId, mMVP);
    setPCData(objectId, lights);
}

void VulkanMaterial::bindDepth(uint64 objectId, const Matrix4x4 &mMVP, const Matrix4x4 &mModel, const Matrix3x3 &mNormal, ModelDataType dataType)
{
    if (dataType == ModelDataType::Unknown)
        return;

    AffectingLights lights{};
    selectPipelineDepth(dataType);
    selectDescriptorDepth(dataType);
    vulkanUtils->getObjectBuffers()->updateObjectData(objectId, mModel, Matrix4x4(mNormal), mMVP);
    setPCData(objectId, lights);
}

void VulkanMaterial::bindColor(uint64 objectId, const AffectingLights &lights, const Matrix4x4 &mMVP, const Matrix4x4 &mModel, const Matrix3x3 &mNormal, ModelDataType dataType)
{
    if (dataType == ModelDataType::Unknown)
        return;

    selectPipelineColor(dataType);
    selectDescriptorColor(dataType);
    vulkanUtils->getObjectBuffers()->updateObjectData(objectId, mModel, Matrix4x4(mNormal), mMVP);
    setPCData(objectId, lights);
}

void VulkanMaterial::selectPipelineDepth(ModelDataType dataType)
{
}

void VulkanMaterial::selectPipelineColor(ModelDataType dataType)
{
}

void VulkanMaterial::selectPipelineShadowDepth(ModelDataType dataType)
{
}

void VulkanMaterial::selectDescriptorColor(ModelDataType dataType)
{
}

void VulkanMaterial::selectDescriptorDepth(ModelDataType dataType)
{
}

void VulkanMaterial::selectDescriptorDepthShadow(ModelDataType dataType, VulkanLightCascadeData *cascadeData)
{
}

void VulkanMaterial::setPCData(uint64 objectId, const AffectingLights &lights)
{
}