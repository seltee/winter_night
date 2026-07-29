#include "features/renderer/vulkan/materials/vulkanMaterialAtmosphere.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/lights/vulkanLightCascadeData.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanRendererState.h"
#include "features/renderer/vulkan/rendererVulkan.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
#include <array>

using namespace wne;

VulkanMaterialAtmosphere::VulkanMaterialAtmosphere(VulkanUtils *vulkanUtils) : VulkanMaterial(vulkanUtils)
{
    colorPipelineDescriptorSets = std::make_unique<VulkanDescriptorSets>(vulkanUtils);
}

VulkanMaterialAtmosphere::~VulkanMaterialAtmosphere()
{
}

void VulkanMaterialAtmosphere::bindDepthShadow(
    uint64 objectId, Renderer *renderer,
    const Matrix4x4 &mMVP,
    const Matrix3x3 &mNormal,
    const UVData &uvData,
    const MeshArmature *meshArmature,
    bool isDoubleSided,
    ModelDataType dataType)
{
    auto state = (VulkanRendererState *)renderer->getState();
    VulkanLightCascadeData *cascadeData = state->getVulkanLightCascadeData();
    if (dataType == ModelDataType::Unknown || !cascadeData)
        return;

    AffectingLights lights{};
    selectPipelineShadowDepth(dataType, meshArmature, isDoubleSided);
    selectDescriptorDepthShadow(dataType, cascadeData);
    cascadeData->updateObjectData(objectId, mMVP);

    MaterialBoneData materialBoneData{};
    setPCData(objectId, lights, uvData, materialBoneData);
}

void VulkanMaterialAtmosphere::bindDepth(
    uint64 objectId,
    const Matrix4x4 &mMVP,
    const Matrix4x4 &mModel,
    const Matrix3x3 &mNormal,
    const UVData &uvData,
    const MeshArmature *meshArmature,
    ModelDataType dataType)
{
    if (dataType == ModelDataType::Unknown)
        return;

    AffectingLights lights{};
    selectPipelineDepth(dataType, meshArmature);
    selectDescriptorDepth(dataType);
    vulkanUtils->getObjectBuffers()->updateObjectData(objectId, mModel, Matrix4x4(mNormal), mMVP);

    MaterialBoneData materialBoneData{};
    setPCData(objectId, lights, uvData, materialBoneData);
}

void VulkanMaterialAtmosphere::bindColor(
    uint64 objectId,
    const AffectingLights &lights,
    const Matrix4x4 &mMVP,
    const Matrix4x4 &mModel,
    const Matrix3x3 &mNormal,
    const UVData &uvData,
    const MeshArmature *meshArmature,
    Texture *radianceMap,
    ModelDataType dataType)
{
    if (dataType == ModelDataType::Unknown)
        return;

    selectPipelineColor(dataType, meshArmature);
    selectDescriptorColor(dataType, reinterpret_cast<VulkanTexture *>(radianceMap));
    vulkanUtils->getObjectBuffers()->updateObjectData(objectId, mModel, Matrix4x4(mNormal), mMVP);

    MaterialBoneData materialBoneData{};
    setPCData(objectId, lights, uvData, materialBoneData);
}

void VulkanMaterialAtmosphere::selectPipelineColor(ModelDataType dataType, const MeshArmature *meshArmature)
{
    if (!colorPipeline)
        buildColorPipeline();
    if (colorPipeline)
        vulkanUtils->bindCustomPipeline(colorPipeline.get());
}

void VulkanMaterialAtmosphere::selectDescriptorColor(ModelDataType dataType, VulkanTexture *radianceMap)
{
    if (lastDescriptorColorBond == this)
        return;
    lastDescriptorColorBond = this;

    auto descriptorSets = colorPipelineDescriptorSets.get();
    if (dataType == ModelDataType::VertexTexturedInd16 || dataType == ModelDataType::VertexTexturedInd32)
    {
        auto commandBuffer = vulkanUtils->getCurrentCommandBuffer()->getCommandBuffer();
        auto pipelineLayout = vulkanUtils->getCurrentPipeline()->getPipelineLayout();

        VkDescriptorSet sets[2] = {descriptorSets->getDescriptorSetTexturedColor(), getDescriptorSetAtmoTexture()};
        if (sets[0] && sets[1])
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 2, sets, 0, nullptr);
    }
}

void VulkanMaterialAtmosphere::setPCData(uint64 objectId, const AffectingLights &lights, const UVData &uvData, const MaterialBoneData &materialBoneData)
{
    auto commandBuffer = vulkanUtils->getCurrentCommandBuffer()->getCommandBuffer();
    auto pipelineLayout = vulkanUtils->getCurrentPipeline()->getPipelineLayout();

    PushConstantObject pco{};
    pco.objectId = objectId;
    pco.lights = lights;
    pco.uvShiftX = 0.0f;
    pco.uvShiftY = 0.0f;
    pco.uvScaleX = 1.0f;
    pco.uvScaleY = 1.0f;
    pco.enableBones = 0;

    vkCmdPushConstants(
        commandBuffer,
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, // Stage flags
        0,                                                         // Offset
        sizeof(PushConstantObject),                                // Size
        &pco                                                       // Data pointer
    );
}

void VulkanMaterialAtmosphere::resetPipeline()
{
    nullifyPipelines();
}

void VulkanMaterialAtmosphere::buildColorPipeline()
{
    // color pipeline
    auto newPipeline = std::make_unique<VulkanPipelineUniversal>(vulkanUtils);
    VulkanPipelineUniversal::Options colorPipelineOptions{};
    colorPipelineOptions.VkMSAASampleCountBit = vulkanUtils->getVkSampleCountFlagBits(vulkanUtils->getMSAASampleCount());
    colorPipelineOptions.blendingMode = ColorBlending::Solid;
    colorPipelineOptions.enableLightning = flagIsLighted;
    colorPipelineOptions.isMainColorPass = true;
    colorPipelineOptions.enableMasked = flagIsMasked;
    colorPipelineOptions.ignoreDepth = true;

    if (!newPipeline->setup(vulkanUtils->getCurrentRenderPass(), colorPipelineOptions))
        newPipeline = nullptr;

    colorPipeline = std::move(newPipeline);
    colorPipelineDescriptorSets->setupColor();
}

void VulkanMaterialAtmosphere::nullifyPipelines()
{
    colorPipeline = nullptr;
}

VkDescriptorSet VulkanMaterialAtmosphere::getDescriptorSetAtmoTexture()
{
    if (descriptorSet)
        return descriptorSet;
    auto device = vulkanUtils->getVulkanDevice()->getDevice();

    auto descriptorSetLayout = vulkanUtils->getDescriptorSetLayoutSampler();
    if (!descriptorSetLayout)
    {
        Logger::log << "Unable to get sampler descriptor set layout" << endl;
        return nullptr;
    }

    VkDescriptorSetLayout vkDescriptorSetLayout = descriptorSetLayout->getDescriptorSetLayout();
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vulkanUtils->getDescriptorPool()->getDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &vkDescriptorSetLayout;

    if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS)
    {
        Logger::log << "failed to allocate descriptor sets!" << endl;
        return nullptr;
    }

    // albedo sampler
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = (VkImageLayout)((VulkanTexture *)atmosphereTexture.get())->getImageLayout();
    imageInfo.imageView = ((VulkanTexture *)atmosphereTexture.get())->getImageView()->getImageView();
    imageInfo.sampler = vulkanUtils->getSampler()->getTextureSampler();

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);

    return descriptorSet;
}
