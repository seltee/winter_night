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

void VulkanMaterial::bind(const Matrix4x4 &mMVP, const Matrix3x3 &mNormal, ModelDataType dataType)
{
    if (dataType == ModelDataType::Unknown)
        return;

    selectPipeline(dataType);
    selectDescriptor(dataType);
    setPCData(mMVP, mNormal);
}

void VulkanMaterial::selectPipeline(ModelDataType dataType)
{
}

void VulkanMaterial::selectDescriptor(ModelDataType dataType)
{
}

void VulkanMaterial::setPCData(const Matrix4x4 &mMVP, const Matrix3x3 &mNormal)
{
}