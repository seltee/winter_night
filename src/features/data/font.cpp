#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "features/data/font.h"
#include "features/loaders/stb_truetype.h"
#include "core/core.h"
#include <cstring>
#include <string>
#include <locale>
#include <codecvt>

using namespace wne;

Font::Font(const std::string &path)
{
    this->path = path;
    int fileSize = getFileByteSize(path.c_str());
    if (fileSize == 0)
        return;

    FILE *file = fopen(path.c_str(), "rb");
    if (!file)
        return;

    unsigned char *buffer = new unsigned char[fileSize];
    fread(buffer, fileSize, 1, file);
    fclose(file);

    stbtt_fontinfo *font = new stbtt_fontinfo();
    fontInfo = font;
    stbtt_InitFont(font, buffer, stbtt_GetFontOffsetForIndex(buffer, 0));

    memset(map, 0, FONT_MAP_SIZE * sizeof(Glyph **));
    bIsReady = true;
}

Font::~Font()
{
    for (auto &glyph : list)
    {
        delete[] glyph.bitmap;
    }
}

std::shared_ptr<Font> Font::create(const std::string &path)
{
    return std::make_shared<Font>(path);
}

Glyph *Font::getGlyph(uint code, uint size)
{
    if (!bIsReady || size <= 0)
        return nullptr;

    if (code >= 0 && code < FONT_MAP_AMOUNT && size < FONT_MAP_SIZE)
    {
        if (!map[size])
        {
            map[size] = new int[FONT_MAP_AMOUNT];
            for (uint i = 0; i < FONT_MAP_AMOUNT; i++)
                map[size][i] = -1;
        }

        if (map[size][code] != -1)
            return &list.at(map[size][code]);

        map[size][code] = createGlyphInList(code, size);
        return &list.at(map[size][code]);
    }

    for (auto &item : list)
    {
        if (item.code == code)
            return &item;
    }

    int glyph = createGlyphInList(code, size);
    return &list.at(glyph);
}

uint Font::measureWidth(const std::string &string, uint size)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    std::u32string text32 = convert.from_bytes(string);

    int width = 0.0f;
    for (auto &c : text32)
    {
        Glyph *glyph = getGlyph(c, size);
        width += glyph->width;
    }
    return width;
}

uint Font::measureHeight(const std::string &string, uint size)
{
    return size;
}

int Font::createGlyphInList(uint code, uint size)
{
    stbtt_fontinfo *font = (stbtt_fontinfo *)fontInfo;

    float scale = stbtt_ScaleForPixelHeight(font, size);
    int sw, sh, width, height;
    stbtt_GetCodepointBitmapBox(font, code, scale, scale, &sw, &sh, &width, &height);
    width += sw;
    height -= sh;

    uint bitmapSize = width * height;
    unsigned char *bitmap = new unsigned char[bitmapSize];
    memset(bitmap, 0, bitmapSize);

    stbtt_MakeCodepointBitmap(font, bitmap, width, height, width, scale, scale, code);
    for (int iy = height - 1; iy >= 0; iy--)
    {
        int shiftY = iy * width;
        for (int ix = width - 1; ix >= 0; ix--)
        {
            int shift = shiftY + ix;
            if (ix == 0 || iy == 0)
                bitmap[shift] = 0;
            else
                bitmap[shift] = bitmap[shift - 1 - width];
        }
    }

    list.push_back({bitmap, code, size, sw - 1, sh - 1, static_cast<uint>(width <= 1 ? size * 2 / 5 : width), static_cast<uint>(height)});

    return list.size() - 1;
}