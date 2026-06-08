// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "core/core.h"
#include <string>
#include <vector>
#include <memory>

struct Glyph
{
    unsigned char *bitmap;
    uint code;
    uint size;
    int shiftX, shiftY;
    uint width, height;
};

namespace wne
{
    const uint FONT_MAP_SIZE = 2048;
    const uint FONT_MAP_AMOUNT = (256 * 256);

    class WNE_API Font
    {
    public:
        Font(const std::string &path);
        ~Font();
        static std::shared_ptr<Font> create(const std::string &path);

        Glyph *getGlyph(uint code, uint size);

        uint measureWidth(const std::string &string, uint size);
        uint measureHeight(const std::string &string, uint size);

        inline bool isReady() { return bIsReady; }

    protected:
        int createGlyphInList(uint code, uint size);

        bool bIsReady = false;
        std::string path;

        void *fontInfo;

        int *map[FONT_MAP_SIZE];
        std::vector<Glyph> list;
    };
};