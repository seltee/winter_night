#pragma once
#include "features/scene/actor.h"
#include "features/ui/uiNode.h"
#include "features/ui/uiNodeContainer.h"
#include "features/ui/uiNodeList.h"
#include "features/ui/uiNodeCenter.h"
#include "features/ui/uiNodeText.h"

namespace wne
{
    class WNE_API ActorUI : public Actor
    {
    public:
        ActorUI(Renderer *renderer);
        ActorUI(Renderer *renderer, float rootWidth, float rootHeight);

        void update(float delta) override final;
        void renderColor() override final;
        RenderPass getRenderPass() override final;

        float getBoundingRadius() override final;

        inline UINodeContainer &getRoot()
        {
            return root;
        }

        inline void setDimensions(float rootWidth, float rootHeight)
        {
            this->rootWidth = rootWidth;
            this->rootHeight = rootHeight;
        }

    protected:
        UINodeContainer root;
        float rootWidth = 1280.0f;
        float rootHeight = 720.0f;
    };
};