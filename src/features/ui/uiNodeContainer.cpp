#include "features/ui/uiNodeContainer.h"
#include <iostream>

using namespace wne;

UINodeContainer::UINodeContainer()
{
}

UINodeContainer::UINodeContainer(UINode *child)
{
}

UINode::ContextTreeNode UINodeContainer::update(const ContextUpdate &context)
{
    prepareNewState();
    if (this->child)
    {
        ContextUpdate nextContext = {context.contextGlobal, context.visible};
        nextContext.x = context.x;
        nextContext.y = context.y;
        nextContext.width = child->getWidth() ? child->getWidth() : context.width;
        nextContext.height = child->getHeight() ? child->getHeight() : context.height;

        auto result = this->child->update(nextContext);
        if (result.hovered)
            return propagateHoverState(std::move(result.hoveredLine), this->child);
    }
    return {isContextHovered(context)};
}

void UINodeContainer::render(const ContextRender &context)
{
    if (this->child)
        this->child->render(context);
}