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
        bool makeFromFiles(const std::string &vertFilePath, const std::string &fragFilePath, VkDevice device);

        inline void *getShaderStages()
        {
            return shaderStages;
        }

    protected:
        VkDevice device;
        VkShaderModule vertShaderModule = nullptr;
        VkShaderModule fragShaderModule = nullptr;

        std::vector<char> readFile(const std::string &path);
        VkShaderModule createShaderModule(const std::vector<char> &code, VkDevice device);

        void *shaderStages; // *VkPipelineShaderStageCreateInfo
    };
}