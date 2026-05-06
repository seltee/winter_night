#include "features/data/image.h"
#include "features/loaders/stb_image.h"
#include <iostream>

using namespace wne;

Image::Image(ImageDataPtr imageData, uint32 width, uint32 height, uint32 bytesPerPixel)
    : imageData(std::move(imageData)), width(width), height(height), bytesPerPixel(bytesPerPixel)
{
}

std::shared_ptr<Image> Image::create(const std::string &path)
{
    int width, height, bytesPerPixel;
    uint8 *data = stbi_load(path.c_str(), &width, &height, &bytesPerPixel, 4);
    if (!data)
    {
        std::cout << "Image loading failed: " << stbi_failure_reason() << std::endl;
        return nullptr;
    }
    if (width <= 0 || height <= 0)
    {
        std::cout << "Image loading failed: wrong image dimensions" << std::endl;
        return nullptr;
    }

    return std::make_shared<Image>(
        ImageDataPtr(data, stbi_image_free),
        (uint32)width,
        (uint32)height,
        4);
}