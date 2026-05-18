#include "features/renderer/vulkan/pipelines/vulkanDescriptorSetLayout.h"
#include "features/renderer/vulkan/vulkanDevice.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <iostream>

using namespace wne;

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice *vulkanDevice)
{
    this->vulkanDevice = vulkanDevice;
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
    if (descriptorSetLayout)
        vkDestroyDescriptorSetLayout(vulkanDevice->getDevice(), descriptorSetLayout, nullptr);
}

bool VulkanDescriptorSetLayout::setupTexturedDepth()
{
    // pipeline layout
    VkDescriptorSetLayoutBinding pipelineBinding[1]{};

    // mvp matrices
    pipelineBinding[0].binding = 0;
    pipelineBinding[0].descriptorCount = 1;
    pipelineBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pipelineBinding[0].pImmutableSamplers = nullptr;
    pipelineBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfoPipeline{};
    layoutInfoPipeline.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfoPipeline.bindingCount = 1;
    layoutInfoPipeline.pBindings = pipelineBinding;

    if (vkCreateDescriptorSetLayout(vulkanDevice->getDevice(), &layoutInfoPipeline, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    {
        std::cout << "failed to create descriptor set layout!" << std::endl;
        return false;
    }

    return true;
}

bool VulkanDescriptorSetLayout::setupTexturedColor()
{

    // pipeline layout
    VkDescriptorSetLayoutBinding pipelineBinding[8]{};

    // mvp matrices
    pipelineBinding[0].binding = 0;
    pipelineBinding[0].descriptorCount = 1;
    pipelineBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pipelineBinding[0].pImmutableSamplers = nullptr;
    pipelineBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    // model matrices
    pipelineBinding[1].binding = 1;
    pipelineBinding[1].descriptorCount = 1;
    pipelineBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pipelineBinding[1].pImmutableSamplers = nullptr;
    pipelineBinding[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    // normal transform matrices
    pipelineBinding[2].binding = 2;
    pipelineBinding[2].descriptorCount = 1;
    pipelineBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pipelineBinding[2].pImmutableSamplers = nullptr;
    pipelineBinding[2].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    // global data
    pipelineBinding[3].binding = 3;
    pipelineBinding[3].descriptorCount = 1;
    pipelineBinding[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pipelineBinding[3].pImmutableSamplers = nullptr;
    pipelineBinding[3].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    // lights data
    pipelineBinding[4].binding = 4;
    pipelineBinding[4].descriptorCount = 1;
    pipelineBinding[4].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pipelineBinding[4].pImmutableSamplers = nullptr;
    pipelineBinding[4].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    // lights MVPs
    pipelineBinding[5].binding = 5;
    pipelineBinding[5].descriptorCount = 1;
    pipelineBinding[5].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pipelineBinding[5].pImmutableSamplers = nullptr;
    pipelineBinding[5].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    // lights shadow textures
    pipelineBinding[6].binding = 6;
    pipelineBinding[6].descriptorCount = 16;
    pipelineBinding[6].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pipelineBinding[6].pImmutableSamplers = nullptr;
    pipelineBinding[6].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // athmosphere texture
    pipelineBinding[7].binding = 7;
    pipelineBinding[7].descriptorCount = 1;
    pipelineBinding[7].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pipelineBinding[7].pImmutableSamplers = nullptr;
    pipelineBinding[7].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfoPipeline{};
    layoutInfoPipeline.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfoPipeline.bindingCount = 8;
    layoutInfoPipeline.pBindings = pipelineBinding;

    if (vkCreateDescriptorSetLayout(vulkanDevice->getDevice(), &layoutInfoPipeline, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    {
        std::cout << "failed to create descriptor set layout!" << std::endl;
        return false;
    }

    return true;
}

bool VulkanDescriptorSetLayout::setupSampler()
{
    // texture layout
    VkDescriptorSetLayoutBinding samplerBinding[1]{};

    // texture sampler
    samplerBinding[0].binding = 0;
    samplerBinding[0].descriptorCount = 1;
    samplerBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerBinding[0].pImmutableSamplers = nullptr;
    samplerBinding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfoSampler{};
    layoutInfoSampler.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfoSampler.bindingCount = 1;
    layoutInfoSampler.pBindings = samplerBinding;

    if (vkCreateDescriptorSetLayout(vulkanDevice->getDevice(), &layoutInfoSampler, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    {
        std::cout << "failed to create descriptor set layout!" << std::endl;
        return false;
    }

    return true;
}