#pragma once
#include "features/ui/uiNode.h"
#include "core/core.h"
#include <memory>

namespace wne
{
    class WNE_API UINodeContainer : public UINode
    {
    public:
        UINodeContainer(const UINodeContainer &) = delete;
        UINodeContainer &operator=(const UINodeContainer &) = delete;

        UINodeContainer();
        UINodeContainer(UINode *child);

        ContextTreeNode update(const ContextUpdate &context) override;
        void render(const ContextRender &context) override;

        inline void setChild(std::shared_ptr<UINode> child)
        {
            this->child = child;
        }

        inline std::shared_ptr<UINode> getChild()
        {
            return child;
        }

    protected:
        std::shared_ptr<UINode> child;
    };
};