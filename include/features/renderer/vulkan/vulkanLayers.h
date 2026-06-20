#pragma once
#include "core/api.h"
#include <vector>

namespace wne
{
    class WNE_API VulkanLayers
    {
    public:
        VulkanLayers();
        ~VulkanLayers();

        int getLayersCount();
        char **getLayersData();
        bool hasLayer(const char *name);
        void dumpLayers();

    protected:
        std::vector<char *> layerNames;
        unsigned int layerCount = 0;
    };
}