#pragma once
#include "features/ui/uiNode.h"
#include "core/core.h"
#include "core/math.h"
#include <memory>

namespace wne
{
    class Mesh;
    class Material;

    class WNE_API UINodeContainer : public UINode
    {
    public:
        struct Decoration
        {
            bool useBackgroundColor;
            uint32 backgroundColor;
        };

        UINodeContainer(const UINodeContainer &) = delete;
        UINodeContainer &operator=(const UINodeContainer &) = delete;

        UINodeContainer();
        UINodeContainer(uint width, uint height);
        UINodeContainer(std::shared_ptr<UINode> child);
        UINodeContainer(std::shared_ptr<UINode> child, uint width, uint height);
        UINodeContainer(std::shared_ptr<UINode> child, const Decoration &decoration);
        UINodeContainer(std::shared_ptr<UINode> child, uint width, uint height, const Decoration &decoration);

        static std::shared_ptr<UINodeContainer> create();
        static std::shared_ptr<UINodeContainer> create(uint width, uint height);
        static std::shared_ptr<UINodeContainer> create(std::shared_ptr<UINode> child);
        static std::shared_ptr<UINodeContainer> create(std::shared_ptr<UINode> child, uint width, uint height);
        static std::shared_ptr<UINodeContainer> create(std::shared_ptr<UINode> child, const Decoration &decoration);
        static std::shared_ptr<UINodeContainer> create(std::shared_ptr<UINode> child, uint width, uint height, const Decoration &decoration);

        ContextTreeNode update(const ContextUpdate &context) override;
        void render(const ContextRender &context) override;

        uint getWidth() override;
        uint getHeight() override;

        inline void setChild(std::shared_ptr<UINode> child)
        {
            this->child = child;
        }

        inline std::shared_ptr<UINode> getChild()
        {
            return child;
        }

        inline void setDecoration(const Decoration &decoration)
        {
            this->decoration = decoration;
            flagDecorationDirty = true;
        }

    protected:
        void setupRendering(Renderer *contextRenderer);

        uint width = 0;
        uint height = 0;

        std::shared_ptr<UINode> child;
        Decoration decoration{};
        bool flagDecorationDirty = true;

        Renderer *renderer = nullptr;
        std::shared_ptr<Mesh> mesh;
        uint64 objectId = 0xffffffff;

        std::shared_ptr<Material> backgroundMaterial;
        Matrix4x4 mBackgroung = Matrix4x4::identity();
    };
};