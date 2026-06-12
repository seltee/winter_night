#include "features/ui/uiNodeButton.h"
#include <iostream>

using namespace wne;

UINodeButton::UINodeButton(
    std::shared_ptr<UINode> nodeMain,
    std::shared_ptr<UINode> nodeHover,
    std::shared_ptr<UINode> nodeDisabled,
    const std::function<void(UINodeButton *)> &onClick,
    const std::function<void(UINodeButton *)> &onRelease,
    const std::function<void(UINodeButton *)> &onHover,
    const std::function<void(UINodeButton *)> &onBlur,
    bool stateDisabled)
{
    this->nodeMain = nodeMain;
    this->nodeHover = nodeHover;
    this->nodeDisabled = nodeDisabled;
    this->onClick = onClick;
    this->onRelease = onRelease;
    this->onHover = onHover;
    this->onBlur = onBlur;
    this->stateDisabled = stateDisabled;
}

std::shared_ptr<UINodeButton> UINodeButton::create(std::shared_ptr<UINode> nodeMain)
{
    return std::make_shared<UINodeButton>(
        nodeMain, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        false);
}

std::shared_ptr<UINodeButton> UINodeButton::create(
    std::shared_ptr<UINode> nodeMain,
    std::shared_ptr<UINode> nodeHover,
    const std::function<void(UINodeButton *)> &onClick)
{
    return std::make_shared<UINodeButton>(
        nodeMain, nodeHover, nullptr,
        onClick, nullptr, nullptr, nullptr,
        false);
}

std::shared_ptr<UINodeButton> UINodeButton::create(
    std::shared_ptr<UINode> nodeMain,
    std::shared_ptr<UINode> nodeHover,
    std::shared_ptr<UINode> nodeDisabled,
    const std::function<void(UINodeButton *)> &onClick,
    bool stateDisabled)
{
    return std::make_shared<UINodeButton>(
        nodeMain, nodeHover, nodeDisabled,
        onClick, nullptr, nullptr, nullptr,
        stateDisabled);
}

std::shared_ptr<UINodeButton> UINodeButton::create(
    std::shared_ptr<UINode> nodeMain,
    std::shared_ptr<UINode> nodeHover,
    std::shared_ptr<UINode> nodeDisabled,
    const std::function<void(UINodeButton *)> &onClick,
    const std::function<void(UINodeButton *)> &onRelease,
    const std::function<void(UINodeButton *)> &onHover,
    const std::function<void(UINodeButton *)> &onBlur,
    bool stateDisabled)
{
    return std::make_shared<UINodeButton>(
        nodeMain, nodeHover, nodeDisabled,
        onClick, onRelease, onHover, onBlur,
        stateDisabled);
}

UINode::ContextTreeNode UINodeButton::update(const ContextUpdate &context)
{
    prepareNewState();
    auto activeNode = getCurrentStateNode();
    if (activeNode)
    {
        ContextUpdate nextContext = {context.contextGlobal};
        nextContext.x = context.x;
        nextContext.y = context.y;
        nextContext.width = activeNode->getWidth() ? activeNode->getWidth() : context.width;
        nextContext.height = activeNode->getHeight() ? activeNode->getHeight() : context.height;

        auto result = activeNode->update(nextContext);
        if (nodeHover && activeNode != nodeHover)
            nodeHover->update(nextContext);

        if (result.hovered)
            return propagateHoverState(std::move(result.hoveredLine), activeNode);
    }
    return {isContextHovered(context)};
}

void UINodeButton::render(const ContextRender &context)
{
    auto activeNode = getCurrentStateNode();
    if (activeNode)
        activeNode->render(context);
}

uint UINodeButton::getWidth()
{
    return getCurrentStateNode()->getWidth();
}

uint UINodeButton::getHeight()
{
    return getCurrentStateNode()->getHeight();
}

bool UINodeButton::pressLeftMouseButton()
{
    if (onClick)
        onClick(this);
    return false;
}

bool UINodeButton::releaseLeftMouseButton()
{
    if (onRelease)
        onRelease(this);
    return false;
}

std::shared_ptr<UINode> UINodeButton::getCurrentStateNode()
{
    if (stateDisabled)
        return nodeDisabled ? nodeDisabled : nodeMain;
    if (stateHovered)
        return nodeHover ? nodeHover : nodeMain;
    return nodeMain;
}