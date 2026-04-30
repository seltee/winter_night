#include "features/renderer/vulkan/vulkanShader.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace WNE;

VulkanShader::VulkanShader()
{
}

VulkanShader::~VulkanShader()
{
    if (vertShaderModule)
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    if (fragShaderModule)
        vkDestroyShaderModule(device, fragShaderModule, nullptr);
}

bool VulkanShader::makeFromFiles(const std::string &vertFilePath, const std::string &fragFilePath, VkDevice device)
{
    this->device = device;

    std::vector<char> vertData, fragData;
    try
    {
        vertData = readFile(vertFilePath);
        fragData = readFile(fragFilePath);
    }
    catch (const std::runtime_error &e)
    {
        printf("Error: %s\n", e.what());
        return false;
    }

    vertShaderModule = createShaderModule(vertData, device);
    fragShaderModule = createShaderModule(fragData, device);

    VkPipelineShaderStageCreateInfo *createInfo = new VkPipelineShaderStageCreateInfo[2];
    // stage 1 - vertex
    createInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    createInfo[0].module = vertShaderModule;
    createInfo[0].pName = "main";

    // stage 2 - fragment
    createInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    createInfo[1].module = fragShaderModule;
    createInfo[1].pName = "main";

    shaderStages = createInfo;

    return true;
}

std::vector<char> VulkanShader::readFile(const std::string &path) // better: take std::string
{
    // get rid of throw
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + path);
    }

    std::streamsize size = file.tellg();

    if (size <= 0)
    {
        throw std::runtime_error("Invalid shader file size");
    }

    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);

    if (!file.read(buffer.data(), size))
    {
        throw std::runtime_error("Failed to read shader file");
    }

    return buffer;
}

VkShaderModule VulkanShader::createShaderModule(const std::vector<char> &code, VkDevice device)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
}