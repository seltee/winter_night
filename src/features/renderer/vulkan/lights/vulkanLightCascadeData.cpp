#include "features/renderer/vulkan/lights/vulkanLightCascadeData.h"
#include "features/renderer/vulkan/vulkanUtils.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"
#include <array>

using namespace wne;

VulkanLightCascadeData::VulkanLightCascadeData(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

VulkanLightCascadeData::~VulkanLightCascadeData()
{
    auto device = vulkanUtils->getVulkanDevice()->getDevice();

    for (const auto &frame : frames)
    {
        if (frame.objectBufferMVP)
            vkDestroyBuffer(device, frame.objectBufferMVP, nullptr);
        if (frame.objectBufferMVPMemory)
            vkFreeMemory(device, frame.objectBufferMVPMemory, nullptr);
    }
}

bool VulkanLightCascadeData::setup(uint32 resolution)
{
    frames.resize(MAX_FRAMES_IN_FLIGHT);
    for (uint frameNum = 0; frameNum < MAX_FRAMES_IN_FLIGHT; frameNum++)
    {
        frames[frameNum].vulkanDepthBuffer = std::make_unique<VulkanDepthBuffer>(vulkanUtils);
        if (!frames[frameNum].vulkanDepthBuffer->setup(resolution, resolution, 1, true))
        {
            Logger::log << "unable to create depth buffer" << endl;
            return false;
        }
        frames[frameNum].vulkanDepthBuffer->transitionToDefined();

        // To store mvp for each object to render
        const uint matrixBufferSize = sizeof(Matrix4x4) * VulkanObjectBuffers::AMOUNT_OF_OBJECTS;
        if (!vulkanUtils->createBuffer(
                matrixBufferSize,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                frames[frameNum].objectBufferMVP,
                frames[frameNum].objectBufferMVPMemory))
        {
            return false;
        }
        vkMapMemory(
            vulkanUtils->getVulkanDevice()->getDevice(),
            frames[frameNum].objectBufferMVPMemory, 0,
            matrixBufferSize, 0,
            (void **)&(frames[frameNum].objectBufferMVPMapped));
    }
    return true;
}

void VulkanLightCascadeData::updateObjectData(uint32 objectId, const Matrix4x4 &mMVP) noexcept
{
    if (objectId < VulkanObjectBuffers::AMOUNT_OF_OBJECTS)
    {
        uint frameNum = vulkanUtils->getCurrentFrame();
        frames[frameNum].objectBufferMVPMapped[objectId] = mMVP;
    }
}

VulkanFrameBuffer *VulkanLightCascadeData::getFrameBuffer(VulkanRenderPass *depthPass)
{
    uint frameNum = vulkanUtils->getCurrentFrame();
    if (!frames[frameNum].vulkanFrameBuffer)
    {
        frames[frameNum].vulkanFrameBuffer = std::make_unique<VulkanFrameBuffer>(vulkanUtils->getVulkanDevice()->getDevice());
        if (!frames[frameNum].vulkanFrameBuffer->setup(depthPass, frames[frameNum].vulkanDepthBuffer.get()))
            return nullptr;
    }
    else
    {
        // check if depth pass is still correct
        if (frames[frameNum].vulkanFrameBuffer->getDepthPass() != depthPass)
        {
            // utils have new one, recreate
            frames[frameNum].vulkanFrameBuffer = std::make_unique<VulkanFrameBuffer>(vulkanUtils->getVulkanDevice()->getDevice());
            if (!frames[frameNum].vulkanFrameBuffer->setup(depthPass, frames[frameNum].vulkanDepthBuffer.get()))
                return nullptr;
        }
    }
    return frames[frameNum].vulkanFrameBuffer.get();
}

VkDescriptorSet VulkanLightCascadeData::getDescriptorSet(
    ModelDataType dataType,
    VkDescriptorSetLayout layoutPipeline)
{
    uint frameNum = vulkanUtils->getCurrentFrame();
    if (frames[frameNum].descriptorSetTextured)
        return frames[frameNum].descriptorSetTextured;

    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    VulkanDescriptorPool *vulkanDescriptorPool = vulkanUtils->getDescriptorPool();
    const uint matrixBufferSize = sizeof(Matrix4x4) * VulkanObjectBuffers::AMOUNT_OF_OBJECTS;

    VkDescriptorSetLayout layouts[1] = {layoutPipeline};

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vulkanDescriptorPool->getDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts;

    if (vkAllocateDescriptorSets(device, &allocInfo, &(frames[frameNum].descriptorSetTextured)) != VK_SUCCESS)
    {
        Logger::log << "unable to create descriptor set" << endl;
        return nullptr;
    }

    VkDescriptorBufferInfo bufferMVPInfo{};
    bufferMVPInfo.buffer = frames[frameNum].objectBufferMVP;
    bufferMVPInfo.offset = 0;
    bufferMVPInfo.range = matrixBufferSize;

    std::array<VkWriteDescriptorSet, 1> writes{};

    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = frames[frameNum].descriptorSetTextured;
    writes[0].dstBinding = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[0].descriptorCount = 1;
    writes[0].pBufferInfo = &bufferMVPInfo;

    vkUpdateDescriptorSets(device, (uint32)writes.size(), writes.data(), 0, nullptr);

    return frames[frameNum].descriptorSetTextured;
}