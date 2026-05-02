#include "features/camera/cameraPerspective.h"

using namespace wne;

CameraPerspective::CameraPerspective()
{
}

CameraPerspective::CameraPerspective(std::shared_ptr<Window> window)
{
}

std::shared_ptr<CameraPerspective> CameraPerspective::create()
{
    return std::make_shared<CameraPerspective>();
}

std::shared_ptr<CameraPerspective> CameraPerspective::create(std::shared_ptr<Window> window)
{
    return std::make_shared<CameraPerspective>(window);
}

void CameraPerspective::setRelatedWindow(std::shared_ptr<Window> window)
{
    this->window = window;
    update();
}

void CameraPerspective::update()
{
    if (window)
    {
        if (width != window->getWidth() || height != window->getHeight())
        {
            width = window->getWidth();
            height = window->getHeight();
            aspectRatio = (float)width / (float)height;
            updateMatrix();
        }
    }
}

void CameraPerspective::updateMatrix()
{
    mProjection = makePerspectiveProjectionMatrix(aspectRatio, fov, nearPlane, farPlane);
}
