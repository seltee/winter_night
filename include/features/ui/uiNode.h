#pragma once
#include "core/core.h"
#include <memory>
#include <vector>

namespace wne
{
    class Renderer;
    class WNE_API UINode
    {
    public:
        struct ContextRender
        {
            Renderer *renderer;
        };

        struct ContextGlobal
        {
            int mouseX, mouseY;
        };

        struct ContextUpdate
        {
            ContextGlobal *contextGlobal;
            int x;
            int y;
            uint width;
            uint height;
        };

        struct ContextTreeNode
        {
            bool hovered;
            std::vector<std::shared_ptr<UINode>> hoveredLine;
        };

        UINode(const UINode &) = delete;
        UINode &operator=(const UINode &) = delete;

        UINode();
        virtual ~UINode();

        bool isContextHovered(const ContextUpdate &context);
        void prepareNewState();
        ContextTreeNode propagateHoverState(std::vector<std::shared_ptr<UINode>> hoveredLine, std::shared_ptr<UINode> newChild);

        virtual ContextTreeNode update(const ContextUpdate &context);
        virtual void render(const ContextRender &context);

        virtual uint getWidth();
        virtual uint getHeight();

        virtual bool pressLeftMouseButton();
        virtual bool releaseLeftMouseButton();

        inline void setStateHovered(bool state)
        {
            stateHovered = state;
        }

        inline bool isStateHovered()
        {
            return stateHovered;
        }

    protected:
        bool stateHovered = false;
    };
};