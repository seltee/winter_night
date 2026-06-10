#include "features/ui/uiNodeRow.h"

using namespace wne;

UINodeRow::UINodeRow()
{
}

std::shared_ptr<UINodeRow> UINodeRow::create()
{
    return std::make_shared<UINodeRow>();
}

std::shared_ptr<UINodeRow> UINodeRow::create(std::vector<std::shared_ptr<UINode>> children)
{
    auto node = std::make_shared<UINodeRow>();
    node->setChildren(children);
    return node;
}

std::shared_ptr<UINodeRow> UINodeRow::create(std::vector<std::shared_ptr<UINode>> children, Layout layout)
{
    auto node = std::make_shared<UINodeRow>();
    node->setChildren(children);
    node->setLayout(layout);
    return node;
}

std::shared_ptr<UINodeRow> UINodeRow::create(std::vector<std::shared_ptr<UINode>> children, Layout layout, Position position)
{
    auto node = std::make_shared<UINodeRow>();
    node->setChildren(children);
    node->setLayout(layout);
    node->setPosition(position);
    return node;
}

UINode::ContextTreeNode UINodeRow::update(const ContextUpdate &context)
{
    prepareNewState();
    if (children.size() == 0)
        return {false};

    uint childrenWidth = 0;
    for (auto &child : children)
    {
        childrenWidth += child->getWidth();
    }
    uint leftWidth = std::max((int)context.width - (int)childrenWidth, 0);
    uint shift = 0;
    uint shiftAddition = 0;

    if (layout == Layout::Middle)
        shift = leftWidth / 2;
    if (layout == Layout::End)
        shift = leftWidth;
    if (layout == Layout::SpaceBetween)
        shiftAddition = leftWidth / (children.size() - 1);
    if (layout == Layout::SpaceAround)
    {
        shiftAddition = leftWidth / (children.size() + 1);
        shift = shiftAddition;
    }

    std::shared_ptr<wne::UINode> *hoveredChild = nullptr;
    std::vector<std::shared_ptr<UINode>> hoveredLine;
    for (auto &child : children)
    {
        int yShift = 0;
        if (position == Position::Middle)
            yShift = ((int)context.height - (int)child->getHeight()) / 2;
        if (position == Position::Bottom)
            yShift = ((int)context.height - (int)child->getHeight());

        ContextUpdate nextContext = {context.contextGlobal};
        nextContext.x = context.x + shift;
        nextContext.y = context.y + yShift + context.height - yShift;
        nextContext.width = child->getWidth() ? child->getWidth() : context.width / children.size();
        nextContext.height = child->getHeight() ? child->getHeight() : context.height;
        auto result = child->update(nextContext);
        if (result.hovered && !hoveredChild)
        {
            hoveredChild = &child;
            hoveredLine = std::move(result.hoveredLine);
        }

        shift += child->getHeight() + shiftAddition;
    }
    if (hoveredChild)
        return propagateHoverState(std::move(hoveredLine), *hoveredChild);
    return {isContextHovered(context)};
}

void UINodeRow::render(const ContextRender &context)
{
    for (auto &child : children)
    {
        child->render(context);
    }
}