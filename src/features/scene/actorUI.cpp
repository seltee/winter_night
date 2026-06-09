#include "features/scene/actorUI.h"
#include "features/scene/actorCamera.h"
#include "features/scene/scene.h"

using namespace wne;

ActorUI::ActorUI(Renderer *renderer) : Actor(renderer)
{
}

ActorUI::ActorUI(Renderer *renderer, float rootWidth, float rootHeight) : Actor(renderer)
{
    this->rootWidth = rootWidth;
    this->rootHeight = rootHeight;
}

void ActorUI::update(float delta)
{
    root.update(-rootWidth / 2, -rootHeight / 2, rootWidth, rootHeight);
}

void ActorUI::renderColor()
{
    UINode::Context context;
    context.renderer = renderer;
    root.render(context);
}

Actor::RenderPass ActorUI::getRenderPass()
{
    return RenderPass::Blended;
}

float ActorUI::getBoundingRadius()
{
    return 10.0f;
}