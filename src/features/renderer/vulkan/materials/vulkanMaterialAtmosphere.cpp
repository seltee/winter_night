#include "features/renderer/vulkan/materials/vulkanMaterialAtmosphere.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/renderer/vulkan/vulkanDescriptorPool.h"
#include "features/renderer/vulkan/lights/vulkanLightCascadeData.h"
#include "features/logger/logger.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <array>

using namespace wne;

VulkanMaterialAtmosphere::VulkanMaterialAtmosphere(VulkanUtils *vulkanUtils) : VulkanMaterial(vulkanUtils)
{
}

VulkanMaterialAtmosphere::~VulkanMaterialAtmosphere()
{
}

void VulkanMaterialAtmosphere::selectPipelineColor(ModelDataType dataType)
{
    vulkanUtils->enablePipelineAtmosphere();
}

void VulkanMaterialAtmosphere::selectDescriptorColor(ModelDataType dataType)
{
    auto descriptorSets = vulkanUtils->getDescriptorSets();
    if (dataType == ModelDataType::VertexColoredInd16 || dataType == ModelDataType::VertexColoredInd32)
    {
    }
    else if (dataType == ModelDataType::VertexTexturedInd16 || dataType == ModelDataType::VertexTexturedInd32)
    {
        auto commandBuffer = vulkanUtils->getCurrentCommandBuffer()->getCommandBuffer();
        auto pipelineLayout = vulkanUtils->getCurrentPipeline()->getPipelineLayout();

        VkDescriptorSet sets[2] = {descriptorSets->getDescriptorSetTexturedColor(), getDescriptorSetAtmoTexture()};
        if (sets[0] && sets[1])
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 2, sets, 0, nullptr);
    }
}

void VulkanMaterialAtmosphere::setPCData(uint64 objectId, const AffectingLights &lights)
{
    auto commandBuffer = vulkanUtils->getCurrentCommandBuffer()->getCommandBuffer();
    auto pipelineLayout = vulkanUtils->getCurrentPipeline()->getPipelineLayout();

    PushConstantObject pco{};
    pco.objectId = objectId;
    pco.lights = lights;

    vkCmdPushConstants(
        commandBuffer,
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, // Stage flags
        0,                                                         // Offset
        sizeof(PushConstantObject),                                // Size
        &pco                                                       // Data pointer
    );
}

VkDescriptorSet VulkanMaterialAtmosphere::getDescriptorSetAtmoTexture()
{
    if (descriptorSet)
        return descriptorSet;
    auto device = vulkanUtils->getVulkanDevice()->getDevice();
    auto pipeline = vulkanUtils->getCurrentPipeline();

    auto descriptorSetLayout = pipeline->getDescriptorSetLayoutSampler();
    if (!descriptorSetLayout){
        Logger::log << "Unable to get descriptor set layout" << endl;
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
