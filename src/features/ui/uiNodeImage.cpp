#include "features/ui/uiNodeImage.h"
#include "features/renderer/renderer.h"
#include "features/logger/logger.h"
#include "features/renderer/texture.h"

using namespace wne;

UINodeImage::UINodeImage(std::shared_ptr<wne::Texture> image)
{
    this->image = image;
}

UINodeImage::UINodeImage(std::shared_ptr<wne::Texture> image, uint width, uint height)
{
    this->image = image;
    this->width = width;
    this->height = height;
}

std::shared_ptr<UINodeImage> UINodeImage::create(std::shared_ptr<wne::Texture> image)
{
    return std::make_shared<UINodeImage>(image);
}

std::shared_ptr<UINodeImage> UINodeImage::create(std::shared_ptr<wne::Texture> image, uint width, uint height)
{
    return std::make_shared<UINodeImage>(image, width, height);
}

UINode::ContextTreeNode UINodeImage::update(const ContextUpdate &context)
{
    prepareNewState();

    if (renderer != context.contextGlobal->renderer)
    {
        renderer = context.contextGlobal->renderer;

        if (mesh)
            mesh->freeObjectId(objectId);
        mesh = renderer->getDefaultPlane();
        objectId = mesh->genNewObjectId();
    }

    if (isDirtyFlag)
    {
        isDirtyFlag = false;

        material = renderer->createFlatMaterial(image);
        material->setColorBlending(wne::ColorBlending::Alpha);
        material->setLighted(false);
    }

    float texWidth = (float)image->getWidth();
    float texHeight = (float)image->getHeight();

    Matrix4x4 newModel = Matrix4x4::translation(
        (float)context.x + texWidth * 0.5f,
        (float)context.y - texHeight * 0.5f,
        0);
    newModel = newModel * Matrix4x4::rotationY(PI);
    mModel = newModel * Matrix4x4::scale(Vector3(texWidth, texHeight, 1.0f));

    bool hovered = context.contextGlobal->mouseX >= context.x &&
                   context.contextGlobal->mouseY <= context.y &&
                   context.contextGlobal->mouseX < context.x + (int)image->getWidth() &&
                   context.contextGlobal->mouseY > context.y - (int)image->getHeight();

    return {hovered};
}

void UINodeImage::render(const ContextRender &context)
{
    if (!renderer || !material || objectId == 0xffffffff)
        return;

    static const Material::UVData uvModifier = {0.0f, 0.0f, 1.0f, 1.0f};
    auto state = renderer->getState();
    AffectingLights lights{};
    material->bindColor(objectId, lights, state->getViewProjectionMatrix() * mModel, mModel, Matrix3x3::identity(), uvModifier, mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

uint UINodeImage::getWidth()
{
    return width ? width : (image ? image->getWidth() : 0);
}

uint UINodeImage::getHeight()
{
    return height ? height : (image ? image->getHeight() : 0);
}
