#pragma once
#include "core/core.h"
#include "features/ui/uiNode.h"

namespace wne
{
    class UINodeText : public UINode
    {
    public:
        void update(uint16 width, uint16 height) override final;
        void setStyle(int fontSize);
        void setText(const char *str);
        
        void updateTextTexture();

    protected:
    };
};