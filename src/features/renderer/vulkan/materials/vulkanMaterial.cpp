#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "features/renderer/vulkan/materials/vulkanMaterialFlat.h"
#include "features/renderer/vulkan/materials/vulkanMaterialAtmosphere.h"
#include "features/renderer/vulkan/pipelines/vulkanPipeline.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanRendererState.h"
#include "features/renderer/vulkan/rendererVulkanNT.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "vulkan/vulkan.h"
#include <array>
#include <iostream>

using namespace wne;

VulkanMaterial *VulkanMaterial::lastDescriptorColorBond = nullptr;

VulkanMaterial::VulkanMaterial(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

std::shared_ptr<MaterialFlat> VulkanMaterial::createFlat(VulkanUtils *vulkanUtils)
{
    std::shared_ptr<VulkanMaterialFlat> material = std::make_shared<VulkanMaterialFlat>(vulkanUtils);
    material->rebuild();
    return material;
}

std::shared_ptr<MaterialFlat> VulkanMaterial::createFlat(VulkanUtils *vulkanUtils, std::shared_ptr<Texture> texture)
{
    std::shared_ptr<VulkanMaterialFlat> material = std::make_shared<VulkanMaterialFlat>(vulkanUtils);
    material->setAlbedoTexture(texture);
    material->rebuild();
    return material;
}

std::shared_ptr<Material> VulkanMaterial::createAtmosphere(VulkanUtils *vulkanUtils, std::shared_ptr<Texture> texture)
{
    std::shared_ptr<VulkanMaterialAtmosphere> material = std::make_shared<VulkanMaterialAtmosphere>(vulkanUtils);
    material->setAtmosphereTexture(texture);
    material->rebuild();
    return material;
}

void VulkanMaterial::selectPipelineDepth(ModelDataType dataType)
{
}

void VulkanMaterial::selectPipelineColor(ModelDataType dataType)
{
}

void VulkanMaterial::selectPipelineShadowDepth(ModelDataType dataType, bool isDoubleSided)
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

void VulkanMaterial::setPCData(uint64 objectId, const AffectingLights &lights, const Material::UVData &uvData, const MaterialBoneData &materialBoneData)
{
}