#pragma once
#include "core/core.h"
#include "features/data/font.h"
#include "features/data/image.h"
#include <memory>

namespace wne
{
    class WNE_API Media
    {
    public:
        std::shared_ptr<Font> getDefaultFont();
        std::shared_ptr<Image> getIconCross();
        std::shared_ptr<Image> getIconCrossHover();

    protected:
        std::shared_ptr<Font> fontDefault;
        std::shared_ptr<Image> imageIconCross;
        std::shared_ptr<Image> imageIconCrossHover;
    };
}