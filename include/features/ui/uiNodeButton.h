#pragma once
#include "features/ui/uiNode.h"
#include <functional>

namespace wne
{
    class WNE_API UINodeButton : public UINode
    {
    public:
        UINodeButton(const UINodeButton &) = delete;
        UINodeButton &operator=(const UINodeButton &) = delete;

        UINodeButton(
            std::shared_ptr<UINode> nodeMain,
            std::shared_ptr<UINode> nodeHover,
            std::shared_ptr<UINode> nodeDisabled,
            const std::function<void(UINodeButton *)> &onClick,
            const std::function<void(UINodeButton *)> &onRelease,
            const std::function<void(UINodeButton *)> &onHover,
            const std::function<void(UINodeButton *)> &onBlur,
            bool stateDisabled);

        static std::shared_ptr<UINodeButton> create(std::shared_ptr<UINode> nodeMain);
        static std::shared_ptr<UINodeButton> create(
            std::shared_ptr<UINode> nodeMain,
            std::shared_ptr<UINode> nodeHover,
            const std::function<void(UINodeButton *)> &onClick);
        static std::shared_ptr<UINodeButton> create(
            std::shared_ptr<UINode> nodeMain,
            std::shared_ptr<UINode> nodeHover,
            std::shared_ptr<UINode> nodeDisabled,
            const std::function<void(UINodeButton *)> &onClick,
            bool stateDisabled);
        static std::shared_ptr<UINodeButton> create(
            std::shared_ptr<UINode> nodeMain,
            std::shared_ptr<UINode> nodeHover,
            std::shared_ptr<UINode> nodeDisabled,
            const std::function<void(UINodeButton *)> &onClick,
            const std::function<void(UINodeButton *)> &onRelease,
            const std::function<void(UINodeButton *)> &onHover,
            const std::function<void(UINodeButton *)> &onBlur,
            bool stateDisabled);

        ContextTreeNode update(const ContextUpdate &context) override;
        void render(const ContextRender &context) override;

        uint getWidth() override;
        uint getHeight() override;

        bool pressLeftMouseButton() override;
        bool releaseLeftMouseButton() override;

        std::shared_ptr<UINode> getCurrentStateNode(bool visible);

        inline void setDisabled(bool stateDisabled)
        {
            this->stateDisabled = stateDisabled;
        }

        inline bool isDisabled()
        {
            return this->stateDisabled;
        }

    protected:
        std::shared_ptr<UINode> nodeMain;
        std::shared_ptr<UINode> nodeHover;
        std::shared_ptr<UINode> nodeDisabled;
        std::function<void(UINodeButton *)> onClick = nullptr;
        std::function<void(UINodeButton *)> onRelease = nullptr;
        std::function<void(UINodeButton *)> onHover = nullptr;
        std::function<void(UINodeButton *)> onBlur = nullptr;
        bool stateDisabled = false;
    };
};
