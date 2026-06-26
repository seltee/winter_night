#include "features/ui/uiNodeCenter.h"
#include <iostream>

using namespace wne;

UINodeCenter::UINodeCenter()
{
}

std::shared_ptr<UINodeCenter> UINodeCenter::create()
{
    return create(nullptr);
}

std::shared_ptr<UINodeCenter> UINodeCenter::create(std::shared_ptr<UINode> child)
{
    auto node = std::make_shared<UINodeCenter>();
    node->setChild(child);
    return node;
}

std::shared_ptr<UINodeCenter> UINodeCenter::create(std::shared_ptr<UINode> child, uint width, uint height)
{
    auto node = std::make_shared<UINodeCenter>();
    node->setChild(child);
    node->setDimensions(width, height);
    return node;
}

UINode::ContextTreeNode UINodeCenter::update(const ContextUpdate &context)
{
    prepareNewState();
    if (child)
    {
        uint selfWidth = width ? width : context.width;
        uint selfHeight = height ? height : context.height;

        uint proptWidth = child->getWidth() ? child->getWidth() : selfWidth;
        uint proptHeight = child->getHeight() ? child->getHeight() : selfHeight;

        ContextUpdate nextContext = {context.contextGlobal, context.visible};
        nextContext.x = context.x + ((int)selfWidth - (int)proptWidth) / 2;
        nextContext.y = context.y - ((int)selfHeight - (int)proptHeight) / 2;
        nextContext.width = proptWidth;
        nextContext.height = proptHeight;

        auto result = child->update(nextContext);
        if (result.hovered)
            return propagateHoverState(std::move(result.hoveredLine), child);
        return {isContextHovered(context)};
    }
    return {isContextHovered(context)};
}

void UINodeCenter::render(const ContextRender &context)
{
    if (child)
        child->render(context);
}

uint UINodeCenter::getWidth()
{
    if (width)
        return width;
    return 0;
}

uint UINodeCenter::getHeight()
{
    if (height)
        return height;
    return 0;
}