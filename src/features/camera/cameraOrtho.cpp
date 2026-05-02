#include "features/camera/cameraOrtho.h"

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
}

CameraOrtho::CameraOrtho(std::shared_ptr<Window> window)
{
    this->window = window;
    update();
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
        updateMatrix();
    }
}

// updates if window is set
void CameraOrtho::update()
{
    if (window)
    {
        if (width != window->getWidth() || height != window->getHeight())
        {
            width = window->getWidth();
            height = window->getHeight();
            updateMatrix();
        }
    }
}

void CameraOrtho::updateMatrix()
{
    int right = width / 2;
    int left = -right;
    int bottom = height / 2;
    int top = -bottom;
    mProjection[0][0] = 2.0f / (right - left);
    mProjection[1][1] = 2.0f / (top - bottom);
    mProjection[2][2] = -2.0f / (far - near);
    mProjection[0][3] = -(right + left) / (right - left);
    mProjection[1][3] = -(top + bottom) / (top - bottom);
    mProjection[2][3] = -(far + near) / (far - near);
    mProjection[3][3] = 1.0f;
}