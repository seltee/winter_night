#include "features/renderer/vulkan/pipelines/vulkanDescriptorSets.h"
#include "features/renderer/vulkan/pipelines/vulkanDescriptorSetLayout.h"
#include "features/renderer/vulkan/pipelines/vulkanPipelineUniversal.h"
#include "features/renderer/vulkan/vulkanShadowMaps.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanSampler.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
#include <array>
#include <cmath>

using namespace wne;

VulkanDescriptorSets::VulkanDescriptorSets(VulkanUtils *vulkanUtils)
{
    this->vulkanDescriptorPool = vulkanUtils->getDescriptorPool();
    this->vulkanDevice = vulkanUtils->getVulkanDevice();
    this->vulkanObjectBuffers = vulkanUtils->getObjectBuffers();
    this->vulkanUtils = vulkanUtils;
}

VulkanDescriptorSets::~VulkanDescriptorSets()
{
}

bool VulkanDescriptorSets::setup()
{
    if (!setupColor())
        return false;
    if (!setupDepth())
        return false;
    return true;
}

bool VulkanDescriptorSets::setupColor()
{
    uint maxFramesInFlight = vulkanObjectBuffers->getFramesMaxInFlight();
    descriptorSetColor.resize(maxFramesInFlight);

    for (uint frame = 0; frame < maxFramesInFlight; frame++)
    {
        if (!initDescriptorSetTexturedColor(
                frame,
                &descriptorSetColor[frame],
                vulkanUtils->getDescriptorSetLayoutColor(),
                vulkanUtils->getDescriptorSetLayoutSampler()))
            return false;
    }
    return true;
}

bool VulkanDescriptorSets::setupDepth()
{
    uint maxFramesInFlight = vulkanObjectBuffers->getFramesMaxInFlight();
    descriptorSetDepth.resize(maxFramesInFlight);

    for (uint32 frame = 0; frame < maxFramesInFlight; frame++)
    {
        if (!initDescriptorSetTexturedDepth(
                frame,
                &descriptorSetDepth[frame],
                vulkanUtils->getDescriptorSetLayoutDepth()))
            return false;
    }
    return true;
}

void VulkanDescriptorSets::updateShadowMap(VulkanShadowMaps *shadowMaps, VulkanSampler *sampler)
{
    uint32 amount = std::min((uint32)16, shadowMaps->getShadowMapsAmount());
    std::array<VkDescriptorImageInfo, 16> imageInfos;
    for (uint32 i = 0; i < 16; i++)
    {
        if (i < amount)
        {
            auto depthBuffer = shadowMaps->getDepthBuffer(i);
            imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            imageInfos[i].imageView = depthBuffer->getDepthImageView();
            imageInfos[i].sampler = sampler->getTextureSampler();
        }
        else
        {
            imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            imageInfos[i].imageView = vulkanObjectBuffers->getDummyDepthBuffer()->getDepthImageView();
            imageInfos[i].sampler = sampler->getTextureSampler();
        }
    }

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptorSetColor[vulkanObjectBuffers->getFrameInFlight()];
    write.dstBinding = 6;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 16;
    write.pImageInfo = imageInfos.data();
    write.dstArrayElement = 0;
    vkUpdateDescriptorSets(vulkanDevice->getDevice(), 1, &write, 0, nullptr);
}

void VulkanDescriptorSets::updateRadianceMap(VulkanTexture *texture, VulkanSampler *sampler)
{
    VkDescriptorImageInfo radianceImageInfo{};
    radianceImageInfo.imageLayout = (VkImageLayout)texture->getImageLayout();
    radianceImageInfo.imageView = texture->getImageView()->getImageView();
    radianceImageInfo.sampler = sampler->getTextureSampler();

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptorSetColor[vulkanObjectBuffers->getFrameInFlight()];
    write.dstBinding = 7;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &radianceImageInfo;
    write.dstArrayElement = 0;
    vkUpdateDescriptorSets(vulkanDevice->getDevice(), 1, &write, 0, nullptr);
}

bool VulkanDescriptorSets::initDescriptorSetTexturedDepth(
    uint frame,
    VkDescriptorSet *descriptorSet,
    VulkanDescriptorSetLayout *descriptorSetLayoutPipeline)
{
    auto device = vulkanDevice->getDevice();

    VkDescriptorSetLayout layouts[1] = {descriptorSetLayoutPipeline->getDescriptorSetLayout()};

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vulkanDescriptorPool->getDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts;

    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSet) != VK_SUCCESS)
    {
        Logger::log << "unable to create descriptor set" << endl;
        return false;
    }

    VkDescriptorBufferInfo bufferMVPInfo{};
    bufferMVPInfo.buffer = vulkanObjectBuffers->getMVPMatricesBuffer(frame);
    bufferMVPInfo.offset = 0;
    bufferMVPInfo.range = vulkanObjectBuffers->getMatrixBufferSize();

    VkDescriptorBufferInfo bufferBonesInfo{};
    bufferBonesInfo.buffer = vulkanObjectBuffers->getBonesBuffer(frame);
    bufferBonesInfo.offset = 0;
    bufferBonesInfo.range = vulkanObjectBuffers->getBonesBufferSize();

    VkDescriptorBufferInfo bufferBoneWeightsInfo{};
    bufferBoneWeightsInfo.buffer = vulkanObjectBuffers->getBoneWeightsBuffer();
    bufferBoneWeightsInfo.offset = 0;
    bufferBoneWeightsInfo.range = vulkanObjectBuffers->getBoneWeightsBufferSize();

    std::array<VkWriteDescriptorSet, 3> writes{};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = *descriptorSet;
    writes[0].dstBinding = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[0].descriptorCount = 1;
    writes[0].pBufferInfo = &bufferMVPInfo;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = *descriptorSet;
    writes[1].dstBinding = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[1].descriptorCount = 1;
    writes[1].pBufferInfo = &bufferBonesInfo;

    writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[2].dstSet = *descriptorSet;
    writes[2].dstBinding = 2;
    writes[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[2].descriptorCount = 1;
    writes[2].pBufferInfo = &bufferBoneWeightsInfo;

    vkUpdateDescriptorSets(device, (uint32)writes.size(), writes.data(), 0, nullptr);
    return true;
}

bool VulkanDescriptorSets::initDescriptorSetTexturedColor(
    uint frame,
    VkDescriptorSet *descriptorSet,
    VulkanDescriptorSetLayout *descriptorSetLayoutPipeline,
    VulkanDescriptorSetLayout *descriptorSetLayoutSampler)
{
    auto device = vulkanDevice->getDevice();

    VkDescriptorSetLayout layouts[2] = {
        descriptorSetLayoutPipeline->getDescriptorSetLayout(),
        descriptorSetLayoutSampler->getDescriptorSetLayout()};

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vulkanDescriptorPool->getDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts;

    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSet) != VK_SUCCESS)
    {
        Logger::log << "unable to create descriptor set" << endl;
        return false;
    }

    VkDescriptorBufferInfo bufferMVPInfo{};
    bufferMVPInfo.buffer = vulkanObjectBuffers->getMVPMatricesBuffer(frame);
    bufferMVPInfo.offset = 0;
    bufferMVPInfo.range = vulkanObjectBuffers->getMatrixBufferSize();

    VkDescriptorBufferInfo bufferModelInfo{};
    bufferModelInfo.buffer = vulkanObjectBuffers->getModelMatricesBuffer(frame);
    bufferModelInfo.offset = 0;
    bufferModelInfo.range = vulkanObjectBuffers->getMatrixBufferSize();

    VkDescriptorBufferInfo bufferNormalInfo{};
    bufferNormalInfo.buffer = vulkanObjectBuffers->getNormalMatricesBuffer(frame);
    bufferNormalInfo.offset = 0;
    bufferNormalInfo.range = vulkanObjectBuffers->getMatrixBufferSize();

    VkDescriptorBufferInfo bufferGlobalDataInfo{};
    bufferGlobalDataInfo.buffer = vulkanObjectBuffers->getGlobalDataBuffer(frame);
    bufferGlobalDataInfo.offset = 0;
    bufferGlobalDataInfo.range = vulkanObjectBuffers->getGlobalDataSize();

    VkDescriptorBufferInfo bufferLightsInfo{};
    bufferLightsInfo.buffer = vulkanObjectBuffers->getLightsDataBuffer(frame);
    bufferLightsInfo.offset = 0;
    bufferLightsInfo.range = vulkanObjectBuffers->getLightsBufferSize();

    VkDescriptorBufferInfo bufferLightMVPs{};
    bufferLightMVPs.buffer = vulkanObjectBuffers->getLightMVPsBuffer(frame);
    bufferLightMVPs.offset = 0;
    bufferLightMVPs.range = vulkanObjectBuffers->getLightMVPsBufferSize();

    VkDescriptorBufferInfo bufferBonesInfo{};
    bufferBonesInfo.buffer = vulkanObjectBuffers->getBonesBuffer(frame);
    bufferBonesInfo.offset = 0;
    bufferBonesInfo.range = vulkanObjectBuffers->getBonesBufferSize();

    VkDescriptorBufferInfo bufferBoneWeightsInfo{};
    bufferBoneWeightsInfo.buffer = vulkanObjectBuffers->getBoneWeightsBuffer();
    bufferBoneWeightsInfo.offset = 0;
    bufferBoneWeightsInfo.range = vulkanObjectBuffers->getBoneWeightsBufferSize();

    std::array<VkWriteDescriptorSet, 8> writes{};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = *descriptorSet;
    writes[0].dstBinding = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[0].descriptorCount = 1;
    writes[0].pBufferInfo = &bufferMVPInfo;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = *descriptorSet;
    writes[1].dstBinding = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[1].descriptorCount = 1;
    writes[1].pBufferInfo = &bufferModelInfo;

    writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[2].dstSet = *descriptorSet;
    writes[2].dstBinding = 2;
    writes[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[2].descriptorCount = 1;
    writes[2].pBufferInfo = &bufferNormalInfo;

    writes[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[3].dstSet = *descriptorSet;
    writes[3].dstBinding = 3;
    writes[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[3].descriptorCount = 1;
    writes[3].pBufferInfo = &bufferGlobalDataInfo;

    writes[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[4].dstSet = *descriptorSet;
    writes[4].dstBinding = 4;
    writes[4].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[4].descriptorCount = 1;
    writes[4].pBufferInfo = &bufferLightsInfo;

    writes[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[5].dstSet = *descriptorSet;
    writes[5].dstBinding = 5;
    writes[5].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[5].descriptorCount = 1;
    writes[5].pBufferInfo = &bufferLightMVPs;

    writes[6].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[6].dstSet = *descriptorSet;
    writes[6].dstBinding = 8;
    writes[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[6].descriptorCount = 1;
    writes[6].pBufferInfo = &bufferBonesInfo;

    writes[7].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[7].dstSet = *descriptorSet;
    writes[7].dstBinding = 9;
    writes[7].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[7].descriptorCount = 1;
    writes[7].pBufferInfo = &bufferBoneWeightsInfo;

    vkUpdateDescriptorSets(device, (uint32)writes.size(), writes.data(), 0, nullptr);
    return true;
}
