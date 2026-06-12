#pragma once
#include "features/ui/uiNode.h"
#include "core/core.h"
#include <memory>
#include <vector>

namespace wne
{
    class WNE_API UINodeRow : public UINode
    {
    public:
        enum class Layout
        {
            Start,
            Middle,
            End,
            SpaceBetween,
            SpaceAround
        };
        enum class Position
        {
            Top,
            Middle,
            Bottom
        };

        UINodeRow(const UINodeRow &) = delete;
        UINodeRow &operator=(const UINodeRow &) = delete;

        UINodeRow();

        static std::shared_ptr<UINodeRow> create();
        static std::shared_ptr<UINodeRow> create(std::vector<std::shared_ptr<UINode>> children);
        static std::shared_ptr<UINodeRow> create(std::vector<std::shared_ptr<UINode>> children, Layout layout);
        static std::shared_ptr<UINodeRow> create(std::vector<std::shared_ptr<UINode>> children, Layout layout, Position position);

        ContextTreeNode update(const ContextUpdate &context) override;
        void render(const ContextRender &context) override;

        uint getWidth() override final;
        uint getHeight() override final;

        inline void setLayout(Layout layout)
        {
            this->layout = layout;
        }

        inline Layout getLayout()
        {
            return layout;
        }

        inline void setPosition(Position position)
        {
            this->position = position;
        }

        inline Position getPosition()
        {
            return position;
        }

        inline void setChildren(std::vector<std::shared_ptr<UINode>> children)
        {
            this->children = children;
        }

    protected:
        std::vector<std::shared_ptr<UINode>> children;
        Layout layout = Layout::Start;
        Position position = Position::Top;
    };
};