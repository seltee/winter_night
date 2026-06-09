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

        void update(int x, int y, uint width, uint height) override;
        void render(Context &context) override;
    };
};