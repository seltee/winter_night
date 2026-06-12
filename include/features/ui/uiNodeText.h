#pragma once
#include "core/core.h"
#include "features/ui/uiNode.h"
#include "features/renderer/text.h"
#include "core/math.h"
#include <memory>

namespace wne
{
    class Renderer;
    class Material;
    class Mesh;

    class WNE_API UINodeText : public UINode
    {
    public:
        UINodeText(const UINodeText &) = delete;
        UINodeText &operator=(const UINodeText &) = delete;

        UINodeText(std::shared_ptr<Font> font);

        static std::shared_ptr<UINodeText> create(std::shared_ptr<Font> font);
        static std::shared_ptr<UINodeText> create(std::shared_ptr<Font> font, const char *text);
        static std::shared_ptr<UINodeText> create(std::shared_ptr<Font> font, const char *text, uint32 fontSize);
        static std::shared_ptr<UINodeText> create(std::shared_ptr<Font> font, const char *text, uint32 fontSize, uint32 color);

        ContextTreeNode update(const ContextUpdate &context) override;
        void render(const ContextRender &context) override;

        uint getWidth() override final;
        uint getHeight() override final;

        inline void setText(const char *text)
        {
            this->text->setText(text);
            isDirtyFlag = true;
        }

        inline void setTextSize(uint32 fontSize)
        {
            this->text->setFontSize(fontSize);
            isDirtyFlag = true;
        }

        inline void setTextColor(uint32 fontColor)
        {
            this->text->setTextColor(fontColor);
            isDirtyFlag = true;
        }

    protected:
        bool isDirtyFlag = true;
        std::shared_ptr<Text> text;
        std::shared_ptr<Font> font;
        std::shared_ptr<Material> material;
        std::shared_ptr<Mesh> mesh;
        Matrix4x4 mModel = Matrix4x4::identity();
        uint64 objectId = 0xffffffff;
        Renderer *renderer = nullptr;
    };
};