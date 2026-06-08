#include "features/renderer/vulkan/vulkanText.h"
#include "features/renderer/vulkan/vulkanTexture.h"
#include "features/logger/logger.h"

using namespace wne;

VulkanText::VulkanText(std::shared_ptr<Font> font, VulkanUtils *vulkanUtils) : Text(font)
{
    this->vulkanUtils = vulkanUtils;
}

std::shared_ptr<wne::VulkanText> VulkanText::create(std::shared_ptr<Font> font, VulkanUtils *vulkanUtils)
{
    return std::make_shared<VulkanText>(font, vulkanUtils);
}

std::shared_ptr<Texture> VulkanText::getTexture()
{
    if (!texture)
        update();
    return texture;
}

void VulkanText::update()
{
    auto bitmapData = createBitmap();

    if (!texture)
        texture = std::make_shared<VulkanTexture>(vulkanUtils);

    if (!texture->setup(bitmapData->bitmap, bitmapData->width, bitmapData->height))
    {
        Logger::log << "failed to make text texture" << endl;
    }
}