#pragma once
#include "utils/api.h"
#include <vector>

namespace WNE
{
    class WNE_API VulkanInstanceExtensions
    {
    public:
        VulkanInstanceExtensions();
        ~VulkanInstanceExtensions();

        int getExtensionsCount();
        char **getExtensionsData();
        bool hasExtension(const char *name);

    protected:
        std::vector<char *> extNames;
        unsigned int extensionCount = 0;
    };
}