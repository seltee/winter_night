#include "features/renderer/text.h"
#include <string>
#include <string.h>
#include <stdio.h>
#include <locale>
#include <codecvt>
#include <iostream>

using namespace wne;

Text::TextRowData::TextRowData(uint8 *bitmap, uint32 width, uint32 height)
{
    this->bitmap = bitmap;
    this->width = width;
    this->height = height;
}

Text::TextRowData::~TextRowData()
{
    if (bitmap)
        delete[] bitmap;
}

Text::Text(std::shared_ptr<Font> font)
{
    this->font = font;
}

std::shared_ptr<Texture> Text::getTexture()
{
    return nullptr;
}

void Text::update()
{
}

float Text::getProportion()
{
    return 1.0f;
}

std::unique_ptr<Text::TextRowData> Text::createBitmap()
{
    if (text.length() == 0)
        return nullptr;

    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    std::u32string text32 = convert.from_bytes(text);
    uint length = text32.length();

    uint width = nextPowerOfTwo(font->measureWidth(text, fontSize) + 2);
    uint height = nextPowerOfTwo(font->measureHeight(text, fontSize) + 8);
    uint bitmapSize = width * height * 4;
    uint8 *data = new uint8[bitmapSize];
    memset(data, 0, bitmapSize);

    textHeight = 0;
    uint glyphShift = 0;
    for (uint i = 0; i < length; i++)
    {
        Glyph *glyph = font->getGlyph(text32[i], fontSize);
        uint glyphWidth = glyph->width;
        uint glyphHeight = glyph->height;
        uint glyphFullHeight = fontSize + glyphHeight + glyph->shiftY;
        if (textHeight < glyphFullHeight)
            textHeight = glyphFullHeight;

        for (uint y = 0; y < glyphHeight; y++)
        {
            uint yPos = fontSize + y + glyph->shiftY;
            if (yPos < height && yPos >= 0)
            {
                for (uint x = 0; x < glyphWidth; x++)
                {
                    uint lineShift = x + glyphShift + glyph->shiftX;
                    if (lineShift >= width)
                        break;

                    uint32 pixelShift = (yPos * width + lineShift) * 4;
                    *((uint32 *)&data[pixelShift]) = color;
                    data[pixelShift + 3] = glyph->bitmap[y * glyphWidth + x];
                }
            }
        }
        glyphShift += glyph->width;
    }
    textWidth = glyphShift;

    this->textureWidth = width;
    this->textureHeight = height;
    return std::make_unique<Text::TextRowData>(data, width, height);
}