#include "features/camera/cameraOrtho.h"
#include <iostream>

using namespace wne;

CameraOrtho::CameraOrtho()
{
    this->width = 320;
    this->height = 240;
    update();
}

CameraOrtho::CameraOrtho(uint32 width, uint32 height)
{
    this->width = width;
    this->height = height;
    update();
}

CameraOrtho::CameraOrtho(std::shared_ptr<Window> window)
{
    this->window = std::move(window);
    update();
}

std::shared_ptr<CameraOrtho> CameraOrtho::create(uint32 width, uint32 height)
{
    return std::make_shared<CameraOrtho>(width, height);
}

std::shared_ptr<CameraOrtho> CameraOrtho::create(std::shared_ptr<Window> window)
{
    return std::make_shared<CameraOrtho>(window);
}

std::shared_ptr<CameraOrtho> CameraOrtho::createHeightBased(std::shared_ptr<Window> window, float fixedHeight)
{
    std::shared_ptr<CameraOrtho> camera = std::make_shared<CameraOrtho>(window);
    camera->setBase(CameraOrtho::Base::Height, fixedHeight);
    return camera;
}

std::shared_ptr<CameraOrtho> CameraOrtho::createWidthBased(std::shared_ptr<Window> window, float fixedWidth)
{
    std::shared_ptr<CameraOrtho> camera = std::make_shared<CameraOrtho>(window);
    camera->setBase(CameraOrtho::Base::Width, fixedWidth);
    return camera;
}

// will automatically update size of orthographics projection upon update
void CameraOrtho::setRelatedWindow(std::shared_ptr<Window> window)
{
    this->window = window;
    update();
}

// ignored if window is set
void CameraOrtho::setSize(uint32 width, uint32 height)
{
    if (!window)
    {
        this->width = width;
        this->height = height;
        update();
    }
}

// updates if window is set
void CameraOrtho::update()
{
    if (window)
    {
        uint32 newWidth, newHeight;
        if (base == CameraOrtho::Base::Width)
        {
            float aspect = (float)window->getWidth() / (float)window->getHeight();
            newWidth = static_cast<uint32>(baseValue);
            newHeight = static_cast<uint32>(baseValue * aspect);
        }
        if (base == CameraOrtho::Base::Height)
        {
            float aspect = (float)window->getWidth() / (float)window->getHeight();
            newWidth = static_cast<uint32>(baseValue * aspect);
            newHeight = static_cast<uint32>(baseValue);
        }
        else
        {
            newWidth = window->getWidth();
            newHeight = window->getHeight();
        }

        if (width != newWidth || height != newHeight)
        {
            width = newWidth;
            height = newHeight;
            updateMatrix();
        }
    }
    else
    {
        updateMatrix();
    }
}

void CameraOrtho::updateMatrix()
{
    float right = (float)width / 2.0f;
    float left = -right;
    float bottom = -(float)height / 2.0f;
    float top = -bottom;

    mProjection = makeOrthographicProjectionMatrix(left, right, top, bottom, near, far);
}