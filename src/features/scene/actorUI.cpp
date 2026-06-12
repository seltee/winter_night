#include "features/scene/actorUI.h"
#include "features/scene/actorCamera.h"
#include "features/scene/scene.h"
#include <iostream>

using namespace wne;

ActorUI::ActorUI(Renderer *renderer, std::shared_ptr<Window> eventWindow) : Actor(renderer)
{
    ActorUI(renderer, eventWindow, 1280, 720);
}

ActorUI::ActorUI(Renderer *renderer, std::shared_ptr<Window> eventWindow, uint rootWidth, uint rootHeight) : Actor(renderer)
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
    bool clickRegistered = false;
    bool releaseRegistered = false;
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
    }

    UINode::ContextGlobal contextGlobal;
    UINode::ContextUpdate contextUpdate;

    contextGlobal.mouseX = mouseX - (int)rootWidth / 2;
    contextGlobal.mouseY = (int)rootHeight - mouseY - (int)rootHeight / 2;

    contextUpdate.contextGlobal = &contextGlobal;
    contextUpdate.x = -(int)rootWidth / 2;
    contextUpdate.y = -(int)rootHeight / 2 - 48;
    contextUpdate.width = rootWidth;
    contextUpdate.height = rootHeight;

    auto result = root.update(contextUpdate);

    for (auto &node : result.hoveredLine)
    {
        node->setStateHovered(true);
    }

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