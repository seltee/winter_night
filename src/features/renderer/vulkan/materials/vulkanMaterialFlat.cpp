#include "features/renderer/vulkan/materials/vulkanMaterialFlat.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/lights/vulkanLightCascadeData.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
#include <array>

using namespace wne;

VulkanMaterialFlat::VulkanMaterialFlat(VulkanUtils *vulkanUtils) : VulkanMaterial(vulkanUtils)
{
}

VulkanMaterialFlat::~VulkanMaterialFlat()
{
}

void VulkanMaterialFlat::selectPipelineDepth(ModelDataType dataType)
{
    vulkanUtils->enablePipelineDepthByType(dataType, flagIsMasked);
}

void VulkanMaterialFlat::selectPipelineColor(ModelDataType dataType)
{
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

void VulkanMaterialFlat::setPCData(uint64 objectId, const AffectingLights &lights, const UVData &uvData)
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

VkDescriptorSet VulkanMaterialFlat::getDescriptorSetFlatTextured()
{
    if (descriptorSet)
        return descriptorSet;
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
    imageInfo.imageLayout = (VkImageLayout)((VulkanTexture *)albedoTexture.get())->getImageLayout();
    imageInfo.imageView = ((VulkanTexture *)albedoTexture.get())->getImageView()->getImageView();
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
