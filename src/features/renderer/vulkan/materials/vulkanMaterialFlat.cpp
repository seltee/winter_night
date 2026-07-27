#include "features/renderer/vulkan/materials/vulkanMaterialFlat.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/lights/vulkanLightCascadeData.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/renderer/vulkan/vulkanRendererState.h"
#include "features/renderer/vulkan/rendererVulkan.h"
#include "features/renderer/vulkan/vulkanMeshArmature.h"
#include "features/renderer/vulkan/vulkanShaderMaker.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
#include <array>

using namespace wne;

VulkanMaterialFlat::VulkanMaterialFlat(VulkanUtils *vulkanUtils) : VulkanMaterial(vulkanUtils)
{
    VulkanShaderMaker shaderMaker;
    shaderMaker.updateShaderCode();
}

VulkanMaterialFlat::~VulkanMaterialFlat()
{
}

void VulkanMaterialFlat::bindDepthShadow(
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
    selectPipelineShadowDepth(dataType, isDoubleSided);
    selectDescriptorDepthShadow(dataType, cascadeData);
    cascadeData->updateObjectData(objectId, mMVP);

    MaterialBoneData materialBoneData{};
    if (meshArmature)
    {
        materialBoneData.enableBones = 1;
        materialBoneData.boneWeightsShift = static_cast<const VulkanMeshArmature *>(meshArmature)->getWeightIndexShift();
        materialBoneData.boneMatrixesShift = static_cast<const VulkanMeshArmature *>(meshArmature)->getMatrixIndexShift();
    }
    setPCData(objectId, lights, uvData, materialBoneData);
}

void VulkanMaterialFlat::bindDepth(
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
    selectPipelineDepth(dataType);
    selectDescriptorDepth(dataType);
    vulkanUtils->getObjectBuffers()->updateObjectData(objectId, mModel, Matrix4x4(mNormal), mMVP);

    MaterialBoneData materialBoneData{};
    if (meshArmature)
    {
        materialBoneData.enableBones = 1;
        materialBoneData.boneWeightsShift = static_cast<const VulkanMeshArmature *>(meshArmature)->getWeightIndexShift();
        materialBoneData.boneMatrixesShift = static_cast<const VulkanMeshArmature *>(meshArmature)->getMatrixIndexShift();
    }
    setPCData(objectId, lights, uvData, materialBoneData);
}

void VulkanMaterialFlat::bindColor(
    uint64 objectId,
    const AffectingLights &lights,
    const Matrix4x4 &mMVP,
    const Matrix4x4 &mModel,
    const Matrix3x3 &mNormal,
    const UVData &uvData,
    const MeshArmature *meshArmature,
    ModelDataType dataType)
{
    if (dataType == ModelDataType::Unknown)
        return;
    if (isPipelineDirty)
        clearPipelines();

    selectPipelineColor(dataType);
    selectDescriptorColor(dataType);
    vulkanUtils->getObjectBuffers()->updateObjectData(objectId, mModel, Matrix4x4(mNormal), mMVP);

    MaterialBoneData materialBoneData{};
    if (meshArmature)
    {
        materialBoneData.enableBones = 1;
        materialBoneData.boneWeightsShift = static_cast<const VulkanMeshArmature *>(meshArmature)->getWeightIndexShift();
        materialBoneData.boneMatrixesShift = static_cast<const VulkanMeshArmature *>(meshArmature)->getMatrixIndexShift();
    }
    setPCData(objectId, lights, uvData, materialBoneData);
}

void VulkanMaterialFlat::selectPipelineDepth(ModelDataType dataType)
{
    vulkanUtils->enablePipelineDepthByType(dataType, flagIsMasked);
}

void VulkanMaterialFlat::selectPipelineColor(ModelDataType dataType)
{
    /*
            if (meshArmature)
    {
        if (!colorPipelineWithBones)
            buildColorPipeline(true);
        if (colorPipelineWithBones)
            vulkanUtils->bindCustomPipeline(colorPipelineWithBones.get());
    }
    else
    {
        if (!colorPipeline)
            buildColorPipeline(false);
        if (colorPipeline)
            vulkanUtils->bindCustomPipeline(colorPipelineWithBones.get());
    }
    */
    vulkanUtils->enablePipelineColorByType(dataType, colorBlending, flagIsLighted);
}

void VulkanMaterialFlat::selectPipelineShadowDepth(ModelDataType dataType, bool isDoubleSided)
{
    vulkanUtils->enablePipelineShadowByType(dataType, flagIsMasked, isDoubleSided);
}

void VulkanMaterialFlat::selectDescriptorColor(ModelDataType dataType)
{
    if (lastDescriptorColorBond == this)
        return;
    lastDescriptorColorBond = this;

    auto descriptorSets = vulkanUtils->getDescriptorSets();
    if (dataType == ModelDataType::VertexColoredInd16 || dataType == ModelDataType::VertexColoredInd32)
    {
    }
    else if (dataType == ModelDataType::VertexTexturedInd16 || dataType == ModelDataType::VertexTexturedInd32)
    {
        auto commandBuffer = vulkanUtils->getCurrentCommandBuffer()->getCommandBuffer();
        auto pipelineLayout = vulkanUtils->getCurrentPipeline()->getPipelineLayout();

        VkDescriptorSet sets[2] = {descriptorSets->getDescriptorSetTexturedColor(), getDescriptorSetFlatTextured()};
        if (sets[0] && sets[1])
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 2, sets, 0, nullptr);
    }
}

void VulkanMaterialFlat::selectDescriptorDepth(ModelDataType dataType)
{
    auto descriptorSets = vulkanUtils->getDescriptorSets();
    if (dataType == ModelDataType::VertexColoredInd16 || dataType == ModelDataType::VertexColoredInd32)
    {
    }
    else if (dataType == ModelDataType::VertexTexturedInd16 || dataType == ModelDataType::VertexTexturedInd32)
    {
        auto commandBuffer = vulkanUtils->getCurrentCommandBuffer()->getCommandBuffer();
        auto pipelineLayout = vulkanUtils->getCurrentPipeline()->getPipelineLayout();

        if (flagIsMasked)
        {
            VkDescriptorSet sets[2] = {descriptorSets->getDescriptorSetTexturedDepth(), getDescriptorSetFlatTextured()};
            if (sets[0] && sets[1])
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 2, sets, 0, nullptr);
        }
        else
        {
            VkDescriptorSet sets[1] = {descriptorSets->getDescriptorSetTexturedDepth()};
            if (sets[0])
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, sets, 0, nullptr);
        }
    }
}

void VulkanMaterialFlat::selectDescriptorDepthShadow(ModelDataType dataType, VulkanLightCascadeData *cascadeData)
{
    if (dataType == ModelDataType::VertexColoredInd16 || dataType == ModelDataType::VertexColoredInd32)
    {
    }
    else if (dataType == ModelDataType::VertexTexturedInd16 || dataType == ModelDataType::VertexTexturedInd32)
    {
        // should be depth
        auto pipeline = vulkanUtils->getCurrentPipeline();
        auto commandBuffer = vulkanUtils->getCurrentCommandBuffer()->getCommandBuffer();
        auto pipelineLayout = pipeline->getPipelineLayout();

        if (flagIsMasked)
        {
            VkDescriptorSet sets[2] = {
                cascadeData->getDescriptorSet(dataType, pipeline->getDescriptorSetLayoutPipeline()->getDescriptorSetLayout()),
                getDescriptorSetFlatTextured()};

            if (sets[0] && sets[1])
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 2, sets, 0, nullptr);
        }
        else
        {
            VkDescriptorSet sets[1] = {cascadeData->getDescriptorSet(dataType, pipeline->getDescriptorSetLayoutPipeline()->getDescriptorSetLayout())};
            if (sets[0])
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, sets, 0, nullptr);
        }
    }
}

void VulkanMaterialFlat::setPCData(uint64 objectId, const AffectingLights &lights, const UVData &uvData, const MaterialBoneData &materialBoneData)
{
    auto commandBuffer = vulkanUtils->getCurrentCommandBuffer()->getCommandBuffer();
    auto pipelineLayout = vulkanUtils->getCurrentPipeline()->getPipelineLayout();

    PushConstantObject pco{};
    pco.objectId = objectId;
    pco.lights = lights;
    pco.normalShadowingFactor = normalShadowingFactor;
    pco.uvShiftX = uvData.shiftX;
    pco.uvShiftY = uvData.shiftY;
    pco.uvScaleX = uvData.scaleX;
    pco.uvScaleY = uvData.scaleY;
    pco.enableBones = materialBoneData.enableBones;
    pco.boneWeightsShift = materialBoneData.boneWeightsShift;
    pco.boneMatrixesShift = materialBoneData.boneMatrixesShift;

    vkCmdPushConstants(
        commandBuffer,
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, // Stage flags
        0,                                                         // Offset
        sizeof(PushConstantObject),                                // Size
        &pco                                                       // Data pointer
    );
}

void VulkanMaterialFlat::rebuild()
{
}

void VulkanMaterialFlat::setAlbedoTexture(std::shared_ptr<Texture> albedoTexture)
{
    this->albedoTexture = albedoTexture;
}

std::shared_ptr<Texture> VulkanMaterialFlat::getAlbedoTexture()
{
    return this->albedoTexture;
}

VkDescriptorSet VulkanMaterialFlat::getDescriptorSetFlatTextured()
{
    auto albedoImageLayout = static_cast<VulkanTexture *>(albedoTexture.get())->getImageLayout();
    auto albedoImageView = static_cast<VulkanTexture *>(albedoTexture.get())->getImageView()->getImageView();
    if (descriptorSet)
    {
        // if texture has the same image layout and view return descriptor
        if (currentImageLayout == albedoImageLayout && currentImageView == albedoImageView)
            return descriptorSet;
    }

    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    auto pipeline = vulkanUtils->getCurrentPipeline();

    auto descriptorSetLayout = pipeline->getDescriptorSetLayoutSampler();
    if (!descriptorSetLayout)
    {
        Logger::log << "Unable to get descriptor set layout sampler" << endl;
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
    imageInfo.imageLayout = static_cast<VkImageLayout>(albedoImageLayout);
    imageInfo.imageView = albedoImageView;
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

    currentImageLayout = imageInfo.imageLayout;
    currentImageView = imageInfo.imageView;

    return descriptorSet;
}

void VulkanMaterialFlat::clearPipelines()
{
    isPipelineDirty = false;
    colorPipeline = nullptr;
    colorPipelineWithBones = nullptr;
}

void VulkanMaterialFlat::buildColorPipeline(bool enableBones)
{
    Logger::log << "BUILD " << vulkanUtils->getMSAASampleCount() << " " << vulkanUtils->getVkSampleCountFlagBits(vulkanUtils->getMSAASampleCount()) << endl;
    // color pipeline
    auto newPipeline = std::make_unique<VulkanPipelineUniversal>(vulkanUtils->getVulkanDevice());
    VulkanPipelineUniversal::Options colorPipelineOptions{};
    colorPipelineOptions.VkMSAASampleCountBit = vulkanUtils->getVkSampleCountFlagBits(vulkanUtils->getMSAASampleCount());
    colorPipelineOptions.blendingMode = colorBlending;
    colorPipelineOptions.enableLightning = flagIsLighted;
    colorPipelineOptions.isMainColorPass = true;
    colorPipelineOptions.enableBones = enableBones;
    if (!newPipeline->setup(vulkanUtils->getCurrentRenderPass(), colorPipelineOptions))
        newPipeline = nullptr;

    if (enableBones)
        colorPipelineWithBones = std::move(newPipeline);
    else
        colorPipeline = std::move(newPipeline);
}
