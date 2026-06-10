#include "features/ui/uiNodeRow.h"

using namespace wne;

UINodeRow::UINodeRow()
{
}

std::shared_ptr<UINodeRow> UINodeRow::create()
{
    return std::make_shared<UINodeRow>();
}

std::shared_ptr<UINodeRow> UINodeRow::create(std::vector<std::shared_ptr<UINode>> children)
{
    auto node = std::make_shared<UINodeRow>();
    node->setChildren(children);
    return node;
}

std::shared_ptr<UINodeRow> UINodeRow::create(std::vector<std::shared_ptr<UINode>> children, Layout layout)
{
    auto node = std::make_shared<UINodeRow>();
    node->setChildren(children);
    node->setLayout(layout);
    return node;
}

std::shared_ptr<UINodeRow> UINodeRow::create(std::vector<std::shared_ptr<UINode>> children, Layout layout, Position position)
{
    auto node = std::make_shared<UINodeRow>();
    node->setChildren(children);
    node->setLayout(layout);
    node->setPosition(position);
    return node;
}

void UINodeRow::update(int x, int y, uint width, uint height)
{
    if (children.size() == 0)
        return;

    uint childrenWidth = 0;
    for (auto &child : children)
    {
        childrenWidth += child->getWidth();
    }
    uint leftWidth = std::max((int)width - (int)childrenWidth, 0);
    uint shift = 0;
    uint shiftAddition = 0;

    if (layout == Layout::Middle)
        shift = leftWidth / 2;
    if (layout == Layout::End)
        shift = leftWidth;
    if (layout == Layout::SpaceBetween)
        shiftAddition = leftWidth / (children.size() - 1);
    if (layout == Layout::SpaceAround)
    {
        shiftAddition = leftWidth / (children.size() + 1);
        shift = shiftAddition;
    }

    for (auto &child : children)
    {
        int yShift = 0;
        if (position == Position::Middle)
            yShift = ((int)height - (int)child->getHeight()) / 2;
        if (position == Position::Bottom)
            yShift = ((int)height - (int)child->getHeight());

        child->update(x + shift, y + yShift + height - yShift, width, height);
        shift += child->getHeight() + shiftAddition;
    }
}

void UINodeRow::render(Context &context)
{
    for (auto &child : children)
    {
        child->render(context);
    }
}