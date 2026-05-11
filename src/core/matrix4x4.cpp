#include "core/math/matrix4x4.h"
#include "core/math.h"

using namespace wne;

Matrix4x4::Matrix4x4(Matrix3x3 m3x3)
{
    m[0] = {m3x3[0][0], m3x3[0][1], m3x3[0][2], 0};
    m[1] = {m3x3[1][0], m3x3[1][1], m3x3[1][2], 0};
    m[2] = {m3x3[2][0], m3x3[2][1], m3x3[2][2], 0};
    m[3] = {0, 0, 0, 1.0f};
}