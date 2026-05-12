#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "features/renderer/vulkan/materials/vulkanMaterialFlat.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/vulkanTexture.h"
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

void VulkanMaterial::bind(uint64 objectId, const AffectingLights &lights, const Matrix4x4 &mMVP, const Matrix3x3 &mNormal, ModelDataType dataType)
{
    if (dataType == ModelDataType::Unknown)
        return;

    auto mModel = Matrix4x4::identity();
    selectPipeline(dataType);
    selectDescriptor(dataType);
    vulkanUtils->getObjectBuffers()->updateObjectData(objectId, mModel, Matrix4x4(mNormal), mMVP);
    setPCData(objectId, lights);
}

void VulkanMaterial::selectPipeline(ModelDataType dataType)
{
}

void VulkanMaterial::selectDescriptor(ModelDataType dataType)
{
}

void VulkanMaterial::setPCData(uint64 objectId, const AffectingLights &lights)
{
}