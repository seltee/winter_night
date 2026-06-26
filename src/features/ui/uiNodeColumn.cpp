#include "features/ui/uiNodeColumn.h"
#include <iostream>

using namespace wne;

UINodeColumn::UINodeColumn()
{
}

std::shared_ptr<UINodeColumn> UINodeColumn::create()
{
    return std::make_shared<UINodeColumn>();
}

std::shared_ptr<UINodeColumn> UINodeColumn::create(std::vector<std::shared_ptr<UINode>> children)
{
    auto node = std::make_shared<UINodeColumn>();
    node->setChildren(children);
    return node;
}

std::shared_ptr<UINodeColumn> UINodeColumn::create(std::vector<std::shared_ptr<UINode>> children, Layout layout)
{
    auto node = std::make_shared<UINodeColumn>();
    node->setChildren(children);
    node->setLayout(layout);
    return node;
}

std::shared_ptr<UINodeColumn> UINodeColumn::create(std::vector<std::shared_ptr<UINode>> children, Layout layout, Position position)
{
    auto node = std::make_shared<UINodeColumn>();
    node->setChildren(children);
    node->setLayout(layout);
    node->setPosition(position);
    return node;
}

UINode::ContextTreeNode UINodeColumn::update(const ContextUpdate &context)
{
    prepareNewState();
    if (children.size() == 0)
        return {false};

    uint childrenHeight = 0;
    for (auto &child : children)
    {
        childrenHeight += child->getHeight();
    }
    uint leftHeight = std::max((int)context.height - (int)childrenHeight, 0);
    int shift = 0;
    uint shiftAddition = 0;

    if (layout == Layout::Middle)
        shift = -leftHeight / 2;
    if (layout == Layout::End)
        shift = -leftHeight;
    if (layout == Layout::SpaceBetween)
        shiftAddition = leftHeight / (children.size() - 1);
    if (layout == Layout::SpaceAround)
    {
        shiftAddition = leftHeight / (children.size() + 1);
        shift = shiftAddition;
    }

    std::shared_ptr<wne::UINode> *hoveredChild = nullptr;
    std::vector<std::shared_ptr<UINode>> hoveredLine;
    for (auto &child : children)
    {
        int xShift = 0;
        if (position == Position::Middle)
            xShift = ((int)context.width - (int)child->getWidth()) / 2;
        if (position == Position::Right)
            xShift = ((int)context.width - (int)child->getWidth());

        ContextUpdate nextContext = {context.contextGlobal, context.visible};
        nextContext.x = context.x + xShift;
        nextContext.y = context.y - shift;
        nextContext.width = child->getWidth() ? child->getWidth() : context.width;
        nextContext.height = child->getHeight() ? child->getHeight() : context.height / children.size();
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

void UINodeColumn::render(const ContextRender &context)
{
    for (auto &child : children)
    {
        child->render(context);
    }
}

uint UINodeColumn::getWidth()
{
    uint width = 0;
    for (auto &child : children)
    {
        width = std::max(width, child->getWidth());
    }
    return width;
}

uint UINodeColumn::getHeight()
{
    uint height = 0;
    for (auto &child : children)
    {
        height += child->getHeight();
    }
    return height;
}