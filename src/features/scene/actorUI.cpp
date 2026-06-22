#include "features/scene/actorUI.h"
#include "features/scene/actorCamera.h"
#include "features/scene/scene.h"
#include <iostream>

using namespace wne;

ActorUI::ActorUI(Renderer *renderer, std::shared_ptr<Window> eventWindow) : Actor(renderer)
{
    ActorUI(renderer, eventWindow.get(), 1280, 720);
}

ActorUI::ActorUI(Renderer *renderer, Window *eventWindow) : Actor(renderer)
{
    ActorUI(renderer, eventWindow, 1280, 720);
}

ActorUI::ActorUI(Renderer *renderer, std::shared_ptr<Window> eventWindow, uint rootWidth, uint rootHeight) : Actor(renderer)
{
    ActorUI(renderer, eventWindow.get(), rootWidth, rootHeight);
}

ActorUI::ActorUI(Renderer *renderer, Window *eventWindow, uint rootWidth, uint rootHeight) : Actor(renderer)
{
    eventsSubscription = eventWindow->subscribe();
    this->rootWidth = rootWidth;
    this->rootHeight = rootHeight;
}

void ActorUI::update(float delta)
{
    if (!eventsSubscription)
        return;

    WindowEvents::WindowEvent event;
    bool clickRegistered = false, releaseRegistered = false, moveUp = false, moveDown = false, moveLeft = false, moveRight = false;

    while (eventsSubscription->getEvent(&event))
    {
        if (event.type == WindowEvents::WindowEventType::MOUSE_MOVE)
        {
            mouseX = event.mouseMove.positionX;
            mouseY = event.mouseMove.positionY;
        }
        else if (event.type == WindowEvents::WindowEventType::MOUSE_BUTTON_DOWN)
        {
            if (event.mouseButton.button == 0)
            {
                clickRegistered = true;
            }
        }
        else if (event.type == WindowEvents::WindowEventType::MOUSE_BUTTON_UP)
        {
            if (event.mouseButton.button == 0)
            {
                releaseRegistered = true;
            }
        }
        else if (event.type == WindowEvents::WindowEventType::KEY_PRESS)
        {
            if (event.key.scancode == 38)
                moveUp = true;
            if (event.key.scancode == 40)
                moveDown = true;
            if (event.key.scancode == 37)
                moveLeft = true;
            if (event.key.scancode == 39)
                moveRight = true;
        }
        else if (event.type == WindowEvents::WindowEventType::GAMEPAD_DIRECTION_PAD)
        {
            auto dir = event.gamepadDirectionPad.direction;
            if (dir == 1 || dir == 2 || dir == 8)
                moveUp = true;
            if (dir == 2 || dir == 3 || dir == 4)
                moveRight = true;
            if (dir == 4 || dir == 5 || dir == 6)
                moveDown = true;
            if (dir == 6 || dir == 7 || dir == 8)
                moveLeft = true;
        }
    }

    UINode::ContextGlobal contextGlobal;
    UINode::ContextUpdate contextUpdate;

    contextGlobal.mouseX = mouseX - (int)rootWidth / 2;
    contextGlobal.mouseY = (int)rootHeight - mouseY - (int)rootHeight / 2;

    contextUpdate.contextGlobal = &contextGlobal;
    contextUpdate.visible = true;
    contextUpdate.x = -(int)rootWidth / 2;
    contextUpdate.y = -(int)rootHeight / 2 - 48;
    contextUpdate.width = rootWidth;
    contextUpdate.height = rootHeight;

    auto result = root.update(contextUpdate);
    if (result.hoveredLine.size() > 0 && eventsSubscription->getLastInputDevice() != WindowEvents::LastInputDevice::Gamepad)
    {
        if (selectedNode && eventsSubscription->getLastInputDevice() == WindowEvents::LastInputDevice::Keyboard)
        {
            for (auto &node : result.hoveredLine)
            {
                if (isNodeExists(contextGlobal.selectableNodes, node.get()))
                {
                    selectedNode = node.get();
                    break;
                }
            }
        }

        for (auto &node : result.hoveredLine)
        {
            node->setStateHovered(true);
        }
    }

    // gamepad selection / input
    if (moveUp)
        selectedNode = moveFocusVertical(true, contextGlobal.selectableNodes, selectedNode);
    if (moveDown)
        selectedNode = moveFocusVertical(false, contextGlobal.selectableNodes, selectedNode);
    if (moveRight)
        selectedNode = moveFocusHorizontal(true, contextGlobal.selectableNodes, selectedNode);
    if (moveLeft)
        selectedNode = moveFocusHorizontal(false, contextGlobal.selectableNodes, selectedNode);

    if (clickRegistered)
    {
        for (auto &node : result.hoveredLine)
        {
            if (!node->pressLeftMouseButton())
                break;
        }
    }
    if (releaseRegistered)
    {
        for (auto &node : result.hoveredLine)
        {
            if (!node->releaseLeftMouseButton())
                break;
        }
    }

    // clear currently selected if doesn't exist anymore
    if (selectedNode && !isNodeExists(contextGlobal.selectableNodes, selectedNode))
    {
        selectedNode = nullptr;
    }
    // if mouse is used no need for selected node
    if (selectedNode && eventsSubscription->getLastInputDevice() == WindowEvents::LastInputDevice::Mouse)
    {
        selectedNode = nullptr;
    }
    // if using gamepad or keyboard there is should be something selected
    if (!selectedNode && contextGlobal.selectableNodes.size() > 0 && eventsSubscription->getLastInputDevice() != WindowEvents::LastInputDevice::Mouse)
    {
        selectedNode = contextGlobal.selectableNodes[0].node;
    }
    // if selected node still exists after all the checks - make it selected
    if (selectedNode)
    {
        selectedNode->setStateHovered(true);
    }
}

void ActorUI::renderColor()
{
    UINode::ContextRender context;
    context.renderer = renderer;
    root.render(context);
}

Actor::RenderPass ActorUI::getRenderPass()
{
    return RenderPass::Blended;
}

float ActorUI::getBoundingRadius()
{
    return 8.0f;
}

bool ActorUI::isNodeExists(std::vector<UINode::ContextSelectableNode> &nodes, UINode *nodeToCheck)
{
    if (!nodeToCheck)
        return false;
    for (auto &collectionNode : nodes)
    {
        if (nodeToCheck == collectionNode.node)
        {

            return true;
        }
    }
    return false;
}

UINode *ActorUI::moveFocusVertical(bool up, std::vector<UINode::ContextSelectableNode> &nodes, UINode *selectedNode)
{
    if (!selectedNode || nodes.size() == 0)
        return nullptr;
    int selectedX = 0, selectedY = 0;
    for (auto &selectableNode : nodes)
    {
        if (selectableNode.node == selectedNode)
        {
            selectedX = selectableNode.centerX;
            selectedY = selectableNode.centerY;
        }
    }

    // find closest in the direction
    UINode *closest = selectedNode;
    int distance = 999999;
    for (auto &selectableNode : nodes)
    {

        if (selectableNode.node == selectedNode)
            continue;
        if (up && (selectableNode.centerY < selectedY - 1))
            continue;
        if (!up && (selectableNode.centerY > selectedY + 1))
            continue;
        int vDistance = abs(selectableNode.centerY - selectedY);
        if (abs(selectableNode.centerX - selectedX) / 2 > vDistance)
            continue;
        if (vDistance < distance)
        {
            distance = vDistance;
            closest = selectableNode.node;
        }
    }
    return closest;
}

UINode *ActorUI::moveFocusHorizontal(bool right, std::vector<UINode::ContextSelectableNode> &nodes, UINode *selectedNode)
{
    if (!selectedNode || nodes.size() == 0)
        return nullptr;
    int selectedX = 0, selectedY = 0;
    for (auto &selectableNode : nodes)
    {
        if (selectableNode.node == selectedNode)
        {
            selectedX = selectableNode.centerX;
            selectedY = selectableNode.centerY;
        }
    }

    // find closest in the direction
    UINode *closest = selectedNode;
    int distance = 999999;
    for (auto &selectableNode : nodes)
    {

        if (selectableNode.node == selectedNode)
            continue;
        if (right && (selectableNode.centerX < selectedX - 1))
            continue;
        if (!right && (selectableNode.centerX > selectedX + 1))
            continue;
        int hDistance = abs(selectableNode.centerY - selectedY);
        if (abs(selectableNode.centerY - selectedY) / 2 > hDistance)
            continue;
        if (hDistance < distance)
        {
            distance = hDistance;
            closest = selectableNode.node;
        }
    }
    return closest;
}