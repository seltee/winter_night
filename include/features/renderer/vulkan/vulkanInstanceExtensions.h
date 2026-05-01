#pragma once
#include "core/api.h"
#include <vector>

namespace wne
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