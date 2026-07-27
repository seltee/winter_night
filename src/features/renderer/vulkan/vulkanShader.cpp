#include "features/renderer/vulkan/vulkanShader.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "vulkan/vulkan.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace wne;

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

    std::vector<int8> vertData, fragData;
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

    if (!vertShaderModule || !fragShaderModule)
        return false;

    shaderStages.resize(2);

    // stage 1 - vertex
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = vertShaderModule;
    shaderStages[0].pName = "main";

    // stage 2 - fragment
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = fragShaderModule;
    shaderStages[1].pName = "main";

    return true;
}

bool VulkanShader::makeFromMemory(const std::vector<int8> &vertexCode, const std::vector<int8> &fragmentCode, VkDevice device)
{
    this->device = device;

    vertShaderModule = createShaderModule(vertexCode, device);
    fragShaderModule = createShaderModule(fragmentCode, device);

    if (!vertShaderModule || !fragShaderModule)
        return false;

    shaderStages.resize(2);

    // stage 1 - vertex
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = vertShaderModule;
    shaderStages[0].pName = "main";

    // stage 2 - fragment
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = fragShaderModule;
    shaderStages[1].pName = "main";

    return true;
}

std::vector<int8> VulkanShader::readFile(const std::string &path)
{
    // todo get rid of throw
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

    std::vector<int8> buffer(size);

    if (!file.read(reinterpret_cast<char *>(buffer.data()), size))
    {
        throw std::runtime_error("Failed to read shader file");
    }

    return buffer;
}

VkShaderModule VulkanShader::createShaderModule(const std::vector<int8> &code, VkDevice device)
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