#pragma once
#include "features/ui/uiNode.h"
#include "core/core.h"
#include <memory>
#include <vector>

namespace wne
{
    class WNE_API UINodeStack : public UINode
    {
    public:
        UINodeStack(const UINodeStack &) = delete;
        UINodeStack &operator=(const UINodeStack &) = delete;

        UINodeStack();

        static std::shared_ptr<UINodeStack> create();
        static std::shared_ptr<UINodeStack> create(std::vector<std::shared_ptr<UINode>> children);

        ContextTreeNode update(const ContextUpdate &context) override;
        void render(const ContextRender &context) override;

        uint getWidth() override final;
        uint getHeight() override final;

        inline void setChildren(std::vector<std::shared_ptr<UINode>> children)
        {
            this->children = children;
        }

    protected:
        std::vector<std::shared_ptr<UINode>> children;
    };
};
