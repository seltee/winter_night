#include "core/math/matrix3x3.h"
#include "core/math.h"

using namespace wne;

Matrix3x3::Matrix3x3(Matrix4x4 &m4x4)
{
    m[0] = {m4x4[0][0], m4x4[1][0], m4x4[2][0]};
    m[1] = {m4x4[0][1], m4x4[1][1], m4x4[2][1]};
    m[2] = {m4x4[0][2], m4x4[1][2], m4x4[2][2]};
}
