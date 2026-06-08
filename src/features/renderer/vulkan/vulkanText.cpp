#include "features/renderer/vulkan/vulkanText.h"

using namespace wne;

VulkanText::VulkanText(VulkanUtils *vulkanUtils)
{
    this->vulkanUtils = vulkanUtils;
}

std::shared_ptr<Texture> VulkanText::getTexture()
{
    if (texture)
        update();
    return texture;
}

void VulkanText::update()
{
    
}