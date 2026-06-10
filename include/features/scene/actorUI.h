#pragma once
#include "features/scene/actor.h"
#include "features/os/window.h"
#include "features/ui/uiNode.h"
#include "features/ui/uiNodeContainer.h"
#include "features/ui/uiNodeColumn.h"
#include "features/ui/uiNodeRow.h"
#include "features/ui/uiNodeCenter.h"
#include "features/ui/uiNodeText.h"

namespace wne
{
    class WNE_API ActorUI : public Actor
    {
    public:
        ActorUI(Renderer *renderer, std::shared_ptr<Window> eventWindow);
        ActorUI(Renderer *renderer, std::shared_ptr<Window> eventWindow, uint rootWidth, uint rootHeight);

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
        std::shared_ptr<wne::WindowEvents> eventsSubscription;
        UINodeContainer root;

        uint rootWidth = 1280;
        uint rootHeight = 720;
        int16 mouseX = 0, mouseY = 0;
    };
};