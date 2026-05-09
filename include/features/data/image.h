#pragma once
#include <memory>
#include "core/core.h"
#include "core/api.h"

namespace wne
{
    class WNE_API Image
    {
    public:
        using ImageDataPtr = std::shared_ptr<uint8>;
        Image(ImageDataPtr imageData, uint32 width, uint32 height, uint32 bytesPerPixel);

        static std::shared_ptr<Image> create(const std::string &path);

        // 0-1 coordinate, combines color by max + nearby pixels through average
        float getAvarage(float x, float y, float pickRadius);

        inline uint8 *getImageData()
        {
            return imageData.get();
        }

        inline uint16 getWidth()
        {
            return width;
        }

        inline uint16 getHeight()
        {
            return height;
        }

    protected:
        ImageDataPtr imageData;
        uint32 width, height, bytesPerPixel;
    };

};
