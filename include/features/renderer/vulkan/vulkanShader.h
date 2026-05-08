#pragma once
#include "features/renderer/vulkan/vulkanDefines.h"
#include <vector>
#include <string>

namespace wne
{
    class VulkanShader
    {
    public:
        VulkanShader();
        ~VulkanShader();

        VulkanShader(const VulkanShader &) = delete;
        VulkanShader &operator=(const VulkanShader &) = delete;

        bool makeFromFiles(const std::string &vertFilePath, const std::string &fragFilePath, VkDevice device);

        inline VkPipelineShaderStageCreateInfo *getShaderStages()
        {
            return shaderStages;
        }

    protected:
        VkDevice device;
        VkShaderModule vertShaderModule = nullptr;
        VkShaderModule fragShaderModule = nullptr;

        std::vector<char> readFile(const std::string &path);
        VkShaderModule createShaderModule(const std::vector<char> &code, VkDevice device);

        VkPipelineShaderStageCreateInfo *shaderStages;
    };
}