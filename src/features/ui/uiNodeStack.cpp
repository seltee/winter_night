#include "features/ui/uiNodeStack.h"

using namespace wne;

UINodeStack::UINodeStack()
{
}

std::shared_ptr<UINodeStack> UINodeStack::create()
{
    return std::make_shared<UINodeStack>();
}

std::shared_ptr<UINodeStack> UINodeStack::create(std::vector<std::shared_ptr<UINode>> children)
{
    auto node = std::make_shared<UINodeStack>();
    node->setChildren(children);
    return node;
}

UINode::ContextTreeNode UINodeStack::update(const ContextUpdate &context)
{
    prepareNewState();
    if (children.size() == 0)
        return {false};

    std::shared_ptr<wne::UINode> *hoveredChild = nullptr;
    std::vector<std::shared_ptr<UINode>> hoveredLine;
    for (auto &child : children)
    {
        uint width = child->getWidth() ? child->getWidth() : context.width;
        uint height = child->getHeight() ? child->getHeight() : context.height;

        ContextUpdate nextContext = {context.contextGlobal, context.visible};
        nextContext.x = context.x;
        nextContext.y = context.y;
        nextContext.width = child->getWidth() ? child->getWidth() : context.width;
        nextContext.height = child->getHeight() ? child->getHeight() : context.height;

        auto result = child->update(nextContext);
        if (result.hovered)
        {
            hoveredChild = &child;
            hoveredLine = std::move(result.hoveredLine);
        }
    }

    if (hoveredChild)
        return propagateHoverState(std::move(hoveredLine), *hoveredChild);
    return {isContextHovered(context)};
}

void UINodeStack::render(const ContextRender &context)
{
    for (auto &child : children)
    {
        child->render(context);
    }
}

uint UINodeStack::getWidth()
{
    return 0;
}

uint UINodeStack::getHeight()
{
    return 0;
}