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

void UINodeCenter::update(int x, int y, uint width, uint height)
{
    if (child)
    {
        uint proptWidth = child->getWidth() ? child->getWidth() : width;
        uint proptHeight = child->getHeight() ? child->getHeight() : height;
        child->update(x + (width - proptWidth) / 2, y + (height - proptHeight) / 2, proptWidth, proptHeight);
    }
}

void UINodeCenter::render(Context &context)
{
    if (child)
        child->render(context);
}