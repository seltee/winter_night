#include "features/ui/uiNode.h"

using namespace wne;

UINode::UINode()
{
}

UINode::~UINode()
{
}

bool UINode::isContextHovered(const ContextUpdate &context)
{
    return context.contextGlobal->mouseX >= context.x &&
           context.contextGlobal->mouseY >= context.y &&
           context.contextGlobal->mouseX < context.x + (int)context.width &&
           context.contextGlobal->mouseY < context.y + (int)context.height;
}

void UINode::prepareNewState()
{
    stateHovered = false;
}

UINode::ContextTreeNode UINode::propagateHoverState(std::vector<std::shared_ptr<UINode>> hoveredLine, std::shared_ptr<UINode> newChild)
{
    hoveredLine.push_back(newChild);
    return {
        true,
        hoveredLine};
}

UINode::ContextTreeNode UINode::update(const ContextUpdate &context)
{
    prepareNewState();
    return {false};
}

void UINode::render(const ContextRender &context)
{
}

uint UINode::getWidth()
{
    return 0;
}

uint UINode::getHeight()
{
    return 0;
}

bool UINode::pressLeftMouseButton()
{
    return true;
}

bool UINode::releaseLeftMouseButton()
{
    return true;
}