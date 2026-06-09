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
        UINodeText(std::shared_ptr<Font> font, const char *text);
        UINodeText(std::shared_ptr<Font> font, const char *text, uint32 fontSize);

        static std::shared_ptr<UINodeText> create(std::shared_ptr<Font> font);
        static std::shared_ptr<UINodeText> create(std::shared_ptr<Font> font, const char *text);
        static std::shared_ptr<UINodeText> create(std::shared_ptr<Font> font, const char *text, uint32 fontSize);

        void update(int x, int y, uint width, uint height) override final;
        void render(Context &context) override final;

        uint getWidth() override final;
        uint getHeight() override final;

    protected:
        std::shared_ptr<Text> text;
        std::shared_ptr<Font> font;
        std::shared_ptr<Material> material;
        std::shared_ptr<Mesh> mesh;
        Matrix4x4 mModel = Matrix4x4::identity();
        uint64 objectId = 0xffffffff;
        Renderer *renderer = nullptr;
        bool needsUpdate = true;
    };
};