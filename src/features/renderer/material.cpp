#include "features/renderer/material.h"

using namespace wne;

void Material::rebuild()
{
}

void Material::bindDepthShadow(
    uint64 objectId,
    Renderer *renderer,
    const Matrix4x4 &mMVP,
    const Matrix3x3 &mNormal,
    const UVData &uvData,
    const MeshArmature *meshArmature,
    bool isDoubleSided,
    ModelDataType dataType)
{
}

void Material::bindDepth(
    uint64 objectId,
    const Matrix4x4 &mMVP,
    const Matrix4x4 &mModel,
    const Matrix3x3 &mNormal,
    const UVData &uvData,
    const MeshArmature *meshArmature,
    ModelDataType dataType)
{
}

void Material::bindColor(
    uint64 objectId,
    const AffectingLights &lights,
    const Matrix4x4 &mMVP,
    const Matrix4x4 &mModel,
    const Matrix3x3 &mNormal,
    const UVData &uvData,
    const MeshArmature *meshArmature,
    Texture *radianceMap,
    ModelDataType dataType)
{
}