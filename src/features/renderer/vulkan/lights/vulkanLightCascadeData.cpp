#include "features/renderer/vulkan/lights/vulkanLightCascadeData.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>
#include <array>

using namespace wne;

VulkanLightCascadeData::VulkanLightCascadeData(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

VulkanLightCascadeData::~VulkanLightCascadeData()
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    if (objectBufferMVP)
        vkDestroyBuffer(device, objectBufferMVP, nullptr);
    if (objectBufferMVPMemory)
        vkFreeMemory(device, objectBufferMVPMemory, nullptr);
}

bool VulkanLightCascadeData::setup(uint32 resolution)
{
    vulkanDepthBuffer = std::make_unique<VulkanDepthBuffer>(vulkanUtils);
    if (!vulkanDepthBuffer->setup(resolution, resolution, 1, true))
    {
        std::cout << "unable to create depth buffer" << std::endl;
        return false;
    }

    // To store mvp for each object to render
    const uint matrixBufferSize = sizeof(Matrix4x4) * VulkanObjectBuffers::AMOUNT_OF_OBJECTS;
    if (!vulkanUtils->createBuffer(
            matrixBufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            objectBufferMVP,
            objectBufferMVPMemory))
    {
        return false;
    }
    vkMapMemory(vulkanUtils->getVulkanDevice()->getDevice(), objectBufferMVPMemory, 0, matrixBufferSize, 0, (void **)&objectBufferMVPMapped);
    return true;
}

void VulkanLightCascadeData::updateObjectData(uint32 objectId, const Matrix4x4 &mMVP) noexcept
{
    if (objectId < VulkanObjectBuffers::AMOUNT_OF_OBJECTS)
    {
        objectBufferMVPMapped[objectId] = mMVP;
    }
}

VulkanFrameBuffer *VulkanLightCascadeData::getFrameBuffer(VulkanRenderPass *depthPass)
{
    if (!vulkanFrameBuffer)
    {
        vulkanFrameBuffer = std::make_unique<VulkanFrameBuffer>(vulkanUtils->getVulkanDevice()->getDevice());
        if (!vulkanFrameBuffer->setup(depthPass, vulkanDepthBuffer.get()))
            return nullptr;
    }
    else
    {
        // check if depth pass is still correct
        if (vulkanFrameBuffer->getDepthPass() != depthPass)
        {
            // utils have new one, recreate
            vulkanFrameBuffer = std::make_unique<VulkanFrameBuffer>(vulkanUtils->getVulkanDevice()->getDevice());
            if (!vulkanFrameBuffer->setup(depthPass, vulkanDepthBuffer.get()))
                return nullptr;
        }
    }
    return vulkanFrameBuffer.get();
}

VkDescriptorSet VulkanLightCascadeData::getDescriptorSet(
    ModelDataType dataType,
    VkDescriptorSetLayout layoutPipeline)
{
    if (descriptorSetTextured)
        return descriptorSetTextured;

    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    VulkanDescriptorPool *vulkanDescriptorPool = vulkanUtils->getDescriptorPool();
    const uint matrixBufferSize = sizeof(Matrix4x4) * VulkanObjectBuffers::AMOUNT_OF_OBJECTS;

    VkDescriptorSetLayout layouts[1] = {layoutPipeline};

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vulkanDescriptorPool->getDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts;

    if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSetTextured) != VK_SUCCESS)
    {
        std::cout << "unable to create descriptor set" << std::endl;
        return nullptr;
    }

    VkDescriptorBufferInfo bufferMVPInfo{};
    bufferMVPInfo.buffer = objectBufferMVP;
    bufferMVPInfo.offset = 0;
    bufferMVPInfo.range = matrixBufferSize;

    std::array<VkWriteDescriptorSet, 1> writes{};

    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = descriptorSetTextured;
    writes[0].dstBinding = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].descriptorCount = 1;
    writes[0].pBufferInfo = &bufferMVPInfo;

    vkUpdateDescriptorSets(device, (uint32)writes.size(), writes.data(), 0, nullptr);

    return descriptorSetTextured;
}