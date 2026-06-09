#pragma once
#include "features/ui/uiNode.h"
#include "core/core.h"
#include <memory>

namespace wne
{
    class WNE_API UINodeList : public UINode
    {
    public:
        UINodeList(const UINodeList &) = delete;
        UINodeList &operator=(const UINodeList &) = delete;

        UINodeList();
        UINodeList(UINode *child);

        void update(int x, int y, uint width, uint height) override;
        void render(Context &context) override;

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