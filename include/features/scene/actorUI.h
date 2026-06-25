#pragma once
#include "features/scene/actor.h"
#include "features/os/window.h"
#include "features/ui/uiNode.h"
#include "features/ui/uiNodeContainer.h"
#include "features/ui/uiNodeColumn.h"
#include "features/ui/uiNodeRow.h"
#include "features/ui/uiNodeCenter.h"
#include "features/ui/uiNodeText.h"
#include "features/ui/uiNodeButton.h"
#include "features/ui/uiNodeImage.h"

namespace wne
{
    class WNE_API ActorUI : public Actor
    {
    public:
        ActorUI(Renderer *renderer, std::shared_ptr<Window> eventWindow);
        ActorUI(Renderer *renderer, Window *eventWindow);
        ActorUI(Renderer *renderer, std::shared_ptr<Window> eventWindow, uint rootWidth, uint rootHeight);
        ActorUI(Renderer *renderer, Window *eventWindow, uint rootWidth, uint rootHeight);

        void update(float delta) override final;
        void renderColor() override final;
        RenderPass getRenderPass() override final;

        float getBoundingRadius() override final;

        bool isNodeExists(std::vector<UINode::ContextSelectableNode> &nodes, UINode *nodeToCheck);

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
        UINode *moveFocusVertical(bool up, std::vector<UINode::ContextSelectableNode> &nodes, UINode *selectedNode);
        UINode *moveFocusHorizontal(bool right, std::vector<UINode::ContextSelectableNode> &nodes, UINode *selectedNode);

        std::shared_ptr<WindowEvents> eventsSubscription;

        UINodeContainer root;
        UINode *selectedNode = nullptr;

        uint rootWidth = 1280;
        uint rootHeight = 720;
        int16 mouseX = 0, mouseY = 0;
    };
};