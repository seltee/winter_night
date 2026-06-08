#pragma once
#include "features/renderer/texture.h"
#include "features/data/font.h"

#include "core/api.h"
#include <memory>
#include <string>

namespace wne
{
    class WNE_API Text
    {
    public:
        class WNE_API TextRowData
        {
        public:
            TextRowData(uint8 *bitmap, uint32 width, uint32 height);
            ~TextRowData();
            uint8 *bitmap;
            uint32 width, height;
        };

        Text(std::shared_ptr<Font> font);

        virtual std::shared_ptr<Texture> getTexture();
        virtual void update();

        std::unique_ptr<TextRowData> createBitmap();

        inline void setText(const std::string &text)
        {
            this->text = text;
        }

        inline std::string getText()
        {
            return text;
        }

        inline void setFontSize(uint fontSize)
        {
            this->fontSize = fontSize;
        }

        inline uint getFontSize()
        {
            return fontSize;
        }

    protected:
        std::string text;
        std::shared_ptr<Font> font;
        uint fontSize = 24;
        uint color = 0xffffffff;
    };
};