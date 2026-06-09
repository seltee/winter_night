#include "features/ui/uiNodeContainer.h"

using namespace wne;

UINodeContainer::UINodeContainer()
{
}

UINodeContainer::UINodeContainer(UINode *child)
{
}

void UINodeContainer::update(int x, int y, uint width, uint height)
{
    if (this->child)
    {
        uint proptWidth = child->getWidth() ? child->getWidth() : width;
        uint proptHeight = child->getHeight() ? child->getHeight() : height;
        this->child->update(x, y, proptWidth, proptHeight);
    }
}

void UINodeContainer::render(Context &context)
{
    if (this->child)
        this->child->render(context);
}