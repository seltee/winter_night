#include "features/ui/uiNodeColumn.h"
#include <iostream>

using namespace wne;

UINodeColumn::UINodeColumn()
{
}

std::shared_ptr<UINodeColumn> UINodeColumn::create()
{
    return std::make_shared<UINodeColumn>();
}

std::shared_ptr<UINodeColumn> UINodeColumn::create(std::vector<std::shared_ptr<UINode>> children)
{
    auto node = std::make_shared<UINodeColumn>();
    node->setChildren(children);
    return node;
}

std::shared_ptr<UINodeColumn> UINodeColumn::create(std::vector<std::shared_ptr<UINode>> children, Layout layout)
{
    auto node = std::make_shared<UINodeColumn>();
    node->setChildren(children);
    node->setLayout(layout);
    return node;
}

std::shared_ptr<UINodeColumn> UINodeColumn::create(std::vector<std::shared_ptr<UINode>> children, Layout layout, Position position)
{
    auto node = std::make_shared<UINodeColumn>();
    node->setChildren(children);
    node->setLayout(layout);
    node->setPosition(position);
    return node;
}

void UINodeColumn::update(int x, int y, uint width, uint height)
{
    if (children.size() == 0)
        return;

    uint childrenHeight = 0;
    for (auto &child : children)
    {
        childrenHeight += child->getHeight();
    }
    uint leftHeight = std::max((int)height - (int)childrenHeight, 0);
    int shift = -childrenHeight;
    uint shiftAddition = 0;

    if (layout == Layout::Middle)
        shift = leftHeight / 2 - height;
    if (layout == Layout::End)
        shift = -height;
    if (layout == Layout::SpaceBetween)
        shiftAddition = leftHeight / (children.size() - 1);
    if (layout == Layout::SpaceAround)
    {
        shiftAddition = leftHeight / (children.size() + 1);
        shift = shiftAddition;
    }

    for (auto &child : children)
    {
        int xShift = 0;
        if (position == Position::Middle)
            xShift = ((int)width - (int)child->getWidth()) / 2;
        if (position == Position::Right)
            xShift = ((int)width - (int)child->getWidth());

        child->update(x + xShift, y - shift, width, height);
        shift += child->getHeight() + shiftAddition;
    }
}

void UINodeColumn::render(Context &context)
{
    for (auto &child : children)
    {
        child->render(context);
    }
}