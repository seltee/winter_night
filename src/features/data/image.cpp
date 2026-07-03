#include "features/data/image.h"
#include "features/loaders/stb_image.h"
#include "features/logger/logger.h"
#include <iostream>
#include <cstring>

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
        Logger::log << "Image loading failed: " << stbi_failure_reason() << endl;
        return nullptr;
    }
    if (width <= 0 || height <= 0)
    {
        Logger::log << "Image loading failed: wrong image dimensions" << endl;
        return nullptr;
    }

    return std::make_shared<Image>(
        ImageDataPtr(data, stbi_image_free),
        (uint32)width,
        (uint32)height,
        4);
}

std::shared_ptr<Image> Image::createFromMemory(const uint8 *imageData, uint width, uint height)
{
    uint sizeInBytes = width * height * 4;
    auto dataCopy = std::shared_ptr<uint8>(
        new uint8[width * height * 4],
        std::default_delete<uint8[]>());
    memcpy(dataCopy.get(), imageData, sizeInBytes);

    return std::make_shared<Image>(dataCopy, width, height, 4);
}

std::shared_ptr<Image> Image::createFromFileInMemory(const uint8 *imageData, uint length)
{
    int width, height, bytesPerPixel;
    uint8 *data = stbi_load_from_memory(imageData, length, &width, &height, &bytesPerPixel, 4);
    if (!data)
    {
        Logger::log << "Image loading failed: " << stbi_failure_reason() << endl;
        return nullptr;
    }
    if (width <= 0 || height <= 0)
    {
        Logger::log << "Image loading failed: wrong image dimensions" << endl;
        return nullptr;
    }

    return std::make_shared<Image>(
        ImageDataPtr(data, stbi_image_free),
        (uint32)width,
        (uint32)height,
        4);
}

float Image::getAvarage(float x, float y, float pickRadius)
{
    int pickPixelsHalfX = (int)((float)width * pickRadius) / 2;
    int pickPixelsHalfY = (int)((float)height * pickRadius) / 2;
    int middleX = (int)((float)width * x);
    int middleY = (int)((float)width * y);

    int xFrom = std::max((middleX - pickPixelsHalfX), 0);
    int xTo = std::min((middleX + pickPixelsHalfX), (int)width);
    int yFrom = std::max((middleY - pickPixelsHalfY), 0);
    int yTo = std::min((middleY + pickPixelsHalfY), (int)height);

    uint8 *data = imageData.get();
    int amountOfPixels = (xTo - xFrom) * (yTo - yFrom);
    float pixelAccMul = 1.0f / (float)amountOfPixels;
    float acc = 0.0f;
    uint8 pixel;

    for (int y = yFrom; y < yTo; y++)
    {
        int yShift = y * width;
        for (int x = xFrom; x < xTo; x++)
        {
            pixel = std::max(std::max(data[(yShift + x) * 4], data[(yShift + x) * 4]), data[(yShift + x) * 4]);
            acc += (float)pixel / 255.0f * pixelAccMul;
        }
    }

    return acc;
}
