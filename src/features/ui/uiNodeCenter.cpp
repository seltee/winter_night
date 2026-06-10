#include "features/ui/uiNodeCenter.h"

using namespace wne;

UINodeCenter::UINodeCenter()
{
}

std::shared_ptr<UINodeCenter> UINodeCenter::create()
{
    return create(nullptr);
}

std::shared_ptr<UINodeCenter> UINodeCenter::create(std::shared_ptr<UINode> child)
{
    auto node = std::make_shared<UINodeCenter>();
    node->setChild(child);
    return node;
}

std::shared_ptr<UINodeCenter> UINodeCenter::create(std::shared_ptr<UINode> child, uint width, uint height)
{
    auto node = std::make_shared<UINodeCenter>();
    node->setChild(child);
    node->setDimensions(width, height);
    return node;
}

void UINodeCenter::update(int x, int y, uint width, uint height)
{
    if (child)
    {
        uint selfWidth = this->width ? this->width : width;
        uint selfHeight = this->height ? this->height : height;

        uint proptWidth = child->getWidth() ? child->getWidth() : selfWidth;
        uint proptHeight = child->getHeight() ? child->getHeight() : selfHeight;
        child->update(x + ((int)selfWidth - (int)proptWidth) / 2, y + ((int)selfHeight - (int)proptHeight) / 2, proptWidth, proptHeight);
    }
}

void UINodeCenter::render(Context &context)
{
    if (child)
        child->render(context);
}

uint UINodeCenter::getWidth()
{
    if (width)
        return width;
    if (child)
        return child->getWidth();
    return 0;
}

uint UINodeCenter::getHeight()
{
    if (height)
        return height;
    if (child)
        return child->getHeight();
    return 0;
}