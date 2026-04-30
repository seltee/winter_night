#include "features/renderer/vulkan/vulkanModelRender.h"
#include <iostream>

using namespace WNE;

VulkanModelRender::VulkanModelRender()
{
}

VulkanModelRender::~VulkanModelRender()
{
}

VulkanModelRender *VulkanModelRender::fromModel(Model *modelRender)
{
    std::cout << "Vukan model created" << std::endl;
    VulkanModelRender *newModel = new VulkanModelRender();
    return newModel;
}
