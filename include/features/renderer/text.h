#pragma once
#include "features/renderer/texture.h"
#include "core/api.h"
#include <memory>
#include <string>

namespace wne
{
    class WNE_API Text
    {
    public:
        virtual std::shared_ptr<Texture> getTexture();
        virtual void update();

        inline void setText(std::string &text)
        {
            this->text = text;
        }

        inline std::string getText()
        {
            return text;
        }

    protected:
        std::string text;
    };
};