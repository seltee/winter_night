#include "features/data/imageAthmo.h"
#include "features/loaders/stb_image.h"
#include <iostream>
#include <memory>

using namespace wne;

ImageAthmo::ImageAthmo(ImageDataPtr imageData, uint32 width, uint32 height, uint32 bytesPerPixel, uint bluredSize)
    : Image(imageData, width, height, bytesPerPixel)
{
    this->bluredSize = bluredSize;
    createBluredData(bluredSize);
}

std::shared_ptr<ImageAthmo> ImageAthmo::create(const std::string &path)
{
    return create(path, 128, 1.8f, 2.9f);
}

std::shared_ptr<ImageAthmo> ImageAthmo::create(const std::string &path, uint bluredSize)
{
    return create(path, bluredSize, 1.8f, 2.9f);
}

std::shared_ptr<ImageAthmo> ImageAthmo::create(const std::string &path, uint bluredSize, float fLdrScale, float fLdrGamma)
{
    stbi_hdr_to_ldr_gamma(fLdrScale);
    stbi_hdr_to_ldr_scale(fLdrGamma);

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

    return std::make_shared<ImageAthmo>(
        ImageDataPtr(data, stbi_image_free),
        (uint32)width,
        (uint32)height,
        4, bluredSize);
}

void ImageAthmo::createBluredData(uint bluredSize)
{
    imageBluredData = std::shared_ptr<uint8>(
        new uint8[bluredSize * bluredSize * bytesPerPixel],
        std::default_delete<uint8[]>());

    float ratioWidth = static_cast<float>(width) / static_cast<float>(bluredSize);
    float ratioHeight = static_cast<float>(height) / static_cast<float>(bluredSize);
    uint8 *tempData = new uint8[bluredSize * bluredSize * 4];

    // change the original resolution into a new buffer
    for (uint iy = 0; iy < bluredSize; iy++)
    {
        for (uint ix = 0; ix < bluredSize; ix++)
        {
            uint shift = (ix + iy * bluredSize) * 4;
            uint originalX = static_cast<int>(static_cast<float>(ix) * ratioWidth) % width;
            uint originalY = static_cast<int>(static_cast<float>(iy) * ratioHeight) % height;
            uint originalShift = (originalX + originalY * width) * 4;

            tempData[shift + 0] = imageData.get()[originalShift + 0];
            tempData[shift + 1] = imageData.get()[originalShift + 1];
            tempData[shift + 2] = imageData.get()[originalShift + 2];
            tempData[shift + 3] = 255;
        }
    }

    int pickSize = 16;
    int pickSize2 = (pickSize + pickSize + 1) * (pickSize + pickSize + 1);

    for (int iy = 0; iy < (int)bluredSize; iy++)
    {
        for (int ix = 0; ix < (int)bluredSize; ix++)
        {
            uint mData[3] = {0, 0, 0};
            int sShift = ix + iy * (int)bluredSize;

            for (int pickY = -pickSize; pickY <= pickSize; pickY++)
            {
                int linePick = (iy + pickY);
                if (linePick >= (int)bluredSize)
                    linePick = (int)bluredSize - 1;
                if (linePick < 0)
                    linePick = 0;

                int linePickShift = linePick * bluredSize;

                for (int pickX = -pickSize; pickX <= pickSize; pickX++)
                {
                    int pixelPick = (ix + pickX);
                    if (pixelPick < 0)
                        pixelPick += (int)bluredSize;
                    if (pixelPick >= (int)bluredSize)
                        pixelPick -= (int)bluredSize;
                    uint fullShift = ((linePickShift) + pixelPick) * 4;
                    mData[0] += tempData[fullShift];
                    mData[1] += tempData[fullShift + 1];
                    mData[2] += tempData[fullShift + 2];
                }
            }
            mData[0] /= pickSize2;
            mData[1] /= pickSize2;
            mData[2] /= pickSize2;
            imageBluredData.get()[sShift * 4] = mData[0];
            imageBluredData.get()[sShift * 4 + 1] = mData[1];
            imageBluredData.get()[sShift * 4 + 2] = mData[2];
            imageBluredData.get()[sShift * 4 + 3] = 255;
        }
    }

    delete[] tempData;
}