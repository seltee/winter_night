#include "features/static/media.h"
#include "features/static/fontVarela.h"
#include "features/static/iconCross.h"

using namespace wne;

std::shared_ptr<Font> Media::getDefaultFont()
{
    if (!fontDefault)
        fontDefault = Font::create(varelaRegular);
    return fontDefault;
}

std::shared_ptr<Image> Media::getIconCross()
{
    if (!imageIconCross)
        imageIconCross = Image::createFromMemory(iconCross, iconCrossSize);
    return imageIconCross;
}

std::shared_ptr<Image> Media::getIconCrossHover()
{
    if (!imageIconCrossHover)
        imageIconCrossHover = Image::createFromMemory(iconCrossHover, iconCrossHoverSize);
    return imageIconCrossHover;
}
