#pragma once
#include "features/ui/uiNodeContainer.h"
#include "core/core.h"
#include <memory>

namespace wne
{
    class WNE_API UINodeCenter : public UINodeContainer
    {
    public:
        UINodeCenter(const UINodeCenter &) = delete;
        UINodeCenter &operator=(const UINodeCenter &) = delete;

        UINodeCenter();

        static std::shared_ptr<UINodeCenter> create();
        static std::shared_ptr<UINodeCenter> create(std::shared_ptr<UINode> child);
        static std::shared_ptr<UINodeCenter> create(std::shared_ptr<UINode> child, uint width, uint height);

        ContextTreeNode update(const ContextUpdate &context) override;
        void render(const ContextRender &context) override;

        uint getWidth() override;
        uint getHeight() override;

        inline void setDimensions(uint width, uint height)
        {
            this->width = width;
            this->height = height;
        }

    protected:
        uint width = 0;
        uint height = 0;
    };
};