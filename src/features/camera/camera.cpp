#include "features/camera/camera.h"

using namespace wne;

const Matrix4x4 &Camera::getProjectionMatrix()
{
    return mProjection;
}

void Camera::update()
{
}