#pragma once
#include "features/data/image.h"

namespace wne
{
    class WNE_API ImageAthmo : public Image
    {
    public:
        ImageAthmo(ImageDataPtr imageData, uint32 width, uint32 height, uint32 bytesPerPixel, uint bluredSize);
        static std::shared_ptr<ImageAthmo> create(const std::string &path);
        static std::shared_ptr<ImageAthmo> create(const std::string &path, uint bluredSize);
        static std::shared_ptr<ImageAthmo> create(const std::string &path, uint bluredSize, float fLdrScale, float fLdrGamma);

        inline std::shared_ptr<Image> getAthmosphereAsImage(){
            return std::make_shared<Image>(imageBluredData, bluredSize, bluredSize, bytesPerPixel);
        }

        inline uint8 *getAthmosphereImageData()
        {
            return imageBluredData.get();
        }

    protected:
        void createBluredData(uint bluredSize);

        ImageDataPtr imageBluredData;
        uint bluredSize = 0;
    };

};
