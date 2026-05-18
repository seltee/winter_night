#include "features/renderer/vulkan/pipelines/vulkanDescriptorSets.h"
#include "features/renderer/vulkan/pipelines/vulkanDescriptorSetLayout.h"
#include "features/renderer/vulkan/pipelines/vulkanPipelineTextured.h"
#include "features/renderer/vulkan/pipelines/vulkanPipelineColored.h"
#include "features/renderer/vulkan/vulkanShadowMaps.h"
#include "features/renderer/vulkan/vulkanDepthBuffer.h"
#include "features/renderer/vulkan/vulkanSampler.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>
#include <array>
#include <cmath>

using namespace wne;

VulkanDescriptorSets::VulkanDescriptorSets(VulkanDevice *vulkanDevice, VulkanDescriptorPool *vulkanDescriptorPool, VulkanObjectBuffers *vulkanObjectBuffers)
{
    this->vulkanDescriptorPool = vulkanDescriptorPool;
    this->vulkanDevice = vulkanDevice;
    this->vulkanObjectBuffers = vulkanObjectBuffers;
}

VulkanDescriptorSets::~VulkanDescriptorSets()
{
}

bool VulkanDescriptorSets::setup(
    VulkanPipelineTextured *pipelineTexturedDepth,
    VulkanPipelineTextured *pipelineTexturedColor)
{
    uint maxFramesInFlight = vulkanObjectBuffers->getFramesMaxInFlight();
    descriptorSetColoredDepth.resize(maxFramesInFlight);
    descriptorSetColoredColor.resize(maxFramesInFlight);
    descriptorSetTexturedDepth.resize(maxFramesInFlight);
    descriptorSetTexturedColor.resize(maxFramesInFlight);

    for (uint32 frame = 0; frame < maxFramesInFlight; frame++)
    {
        if (!initDescriptorSetColoredDepth(frame, &descriptorSetColoredDepth[frame]))
            return false;

        if (!initDescriptorSetColoredColor(frame, &descriptorSetColoredColor[frame]))
            return false;

        if (!initDescriptorSetTexturedDepth(
                frame,
                &descriptorSetTexturedDepth[frame],
                pipelineTexturedDepth->getDescriptorSetLayoutPipeline()))
            return false;

        if (!initDescriptorSetTexturedColor(
                frame,
                &descriptorSetTexturedColor[frame],
                pipelineTexturedColor->getDescriptorSetLayoutPipeline(),
                pipelineTexturedColor->getDescriptorSetLayoutSampler()))
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
    write.dstSet = descriptorSetTexturedColor[vulkanObjectBuffers->getFrameInFlight()];
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
    write.dstSet = descriptorSetTexturedColor[vulkanObjectBuffers->getFrameInFlight()];
    write.dstBinding = 7;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &radianceImageInfo;
    write.dstArrayElement = 0;
    vkUpdateDescriptorSets(vulkanDevice->getDevice(), 1, &write, 0, nullptr);
}

bool VulkanDescriptorSets::initDescriptorSetColoredDepth(uint frame, VkDescriptorSet *descriptorSet)
{
    *descriptorSet = nullptr;
    return true;
}

bool VulkanDescriptorSets::initDescriptorSetColoredColor(uint frame, VkDescriptorSet *descriptorSet)
{
    *descriptorSet = nullptr;
    return true;
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
        std::cout << "unable to create descriptor set" << std::endl;
        return false;
    }

    VkDescriptorBufferInfo bufferMVPInfo{};
    bufferMVPInfo.buffer = vulkanObjectBuffers->getMVPMatricesBuffer(frame);
    bufferMVPInfo.offset = 0;
    bufferMVPInfo.range = vulkanObjectBuffers->getMatrixBufferSize();

    std::array<VkWriteDescriptorSet, 1> writes{};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = *descriptorSet;
    writes[0].dstBinding = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].descriptorCount = 1;
    writes[0].pBufferInfo = &bufferMVPInfo;

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
        std::cout << "unable to create descriptor set" << std::endl;
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

    std::array<VkWriteDescriptorSet, 6> writes{};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = *descriptorSet;
    writes[0].dstBinding = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].descriptorCount = 1;
    writes[0].pBufferInfo = &bufferMVPInfo;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = *descriptorSet;
    writes[1].dstBinding = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[1].descriptorCount = 1;
    writes[1].pBufferInfo = &bufferModelInfo;

    writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[2].dstSet = *descriptorSet;
    writes[2].dstBinding = 2;
    writes[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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

    vkUpdateDescriptorSets(device, (uint32)writes.size(), writes.data(), 0, nullptr);
    return true;
}
