#include "features/ui/uiNodeContainer.h"
#include "features/renderer/renderer.h"
#include "features/renderer/mesh.h"
#include "features/renderer/material.h"
#include "features/logger/logger.h"
#include <iostream>

using namespace wne;

UINodeContainer::UINodeContainer()
{
}

UINodeContainer::UINodeContainer(uint width, uint height)
{
    this->width = width;
    this->height = height;
}

UINodeContainer::UINodeContainer(std::shared_ptr<UINode> child)
{
    this->child = std::move(child);
}

UINodeContainer::UINodeContainer(std::shared_ptr<UINode> child, uint width, uint height)
{
    this->child = std::move(child);
    this->width = width;
    this->height = height;
}

UINodeContainer::UINodeContainer(std::shared_ptr<UINode> child, const Decoration &decoration)
{
    this->child = std::move(child);
    this->decoration = decoration;
}

UINodeContainer::UINodeContainer(std::shared_ptr<UINode> child, uint width, uint height, const Decoration &decoration)
{
    this->child = std::move(child);
    this->width = width;
    this->height = height;
    this->decoration = decoration;
}

std::shared_ptr<UINodeContainer> UINodeContainer::create()
{
    return std::make_shared<UINodeContainer>();
}

std::shared_ptr<UINodeContainer> UINodeContainer::create(uint width, uint height)
{
    return std::make_shared<UINodeContainer>(width, height);
}

std::shared_ptr<UINodeContainer> UINodeContainer::create(std::shared_ptr<UINode> child)
{
    return std::make_shared<UINodeContainer>(std::move(child));
}

std::shared_ptr<UINodeContainer> UINodeContainer::create(std::shared_ptr<UINode> child, uint width, uint height)
{
    return std::make_shared<UINodeContainer>(std::move(child), width, height);
}

std::shared_ptr<UINodeContainer> UINodeContainer::create(std::shared_ptr<UINode> child, const Decoration &decoration)
{
    return std::make_shared<UINodeContainer>(std::move(child), decoration);
}

std::shared_ptr<UINodeContainer> UINodeContainer::create(std::shared_ptr<UINode> child, uint width, uint height, const Decoration &decoration)
{
    return std::make_shared<UINodeContainer>(std::move(child), width, height, decoration);
}

UINode::ContextTreeNode UINodeContainer::update(const ContextUpdate &context)
{
    prepareNewState();

    if (flagDecorationDirty)
    {
        flagDecorationDirty = false;

        auto renderer = context.contextGlobal->renderer;
        if (decoration.useBackgroundColor)
        {
            setupRendering(renderer);
            auto textureData = std::shared_ptr<uint8>(
                new uint8[2 * 2 * 4],
                std::default_delete<uint8[]>());
            for (uint i = 0; i < 4; i++)
                ((uint32 *)textureData.get())[i] = decoration.backgroundColor;

            auto backgroundImage = std::make_shared<Image>(textureData, 2, 2, 4);
            auto backgroundTexture = renderer->createTexture(backgroundImage);
            backgroundMaterial = renderer->createFlatMaterial(backgroundTexture);
            backgroundMaterial->setColorBlending(wne::ColorBlending::Alpha);
            backgroundMaterial->setLighted(false);
        }
        else
        {
            backgroundMaterial = nullptr;
        }
    }

    uint selfWidth = width ? width : context.width;
    uint selfHeight = height ? height : context.height;

    ContextTreeNode childTreeNode{};
    if (child)
    {
        selfWidth = (!width && child->getWidth()) ? child->getWidth() : selfWidth;
        selfHeight = (!height && child->getHeight()) ? child->getHeight() : selfHeight;

        ContextUpdate nextContext = {context.contextGlobal, context.visible};
        nextContext.x = context.x;
        nextContext.y = context.y;
        nextContext.width = selfWidth;
        nextContext.height = selfHeight;

        childTreeNode = this->child->update(nextContext);
    }

    if (backgroundMaterial)
    {
        float texWidth = (float)selfWidth;
        float texHeight = (float)selfHeight;

        Matrix4x4 newModel = Matrix4x4::translation(
            (float)context.x + texWidth * 0.5f,
            (float)context.y + texHeight * 0.5f,
            0);
        newModel = newModel * Matrix4x4::rotationY(PI);
        mBackgroung = newModel * Matrix4x4::scale(Vector3(texWidth, texHeight, 1.0f));
    }

    if (child && childTreeNode.hovered)
        return propagateHoverState(std::move(childTreeNode.hoveredLine), child);

    bool hovered = context.contextGlobal->mouseX >= context.x &&
                   context.contextGlobal->mouseY >= context.y &&
                   context.contextGlobal->mouseX < context.x + (int)selfWidth &&
                   context.contextGlobal->mouseY < context.y + (int)selfHeight;

    return {hovered};
}

void UINodeContainer::render(const ContextRender &context)
{
    if (backgroundMaterial)
    {
        auto state = renderer->getState();
        AffectingLights lights{};
        backgroundMaterial->bindColor(
            objectId,
            lights,
            state->getViewProjectionMatrix() * mBackgroung,
            mBackgroung,
            Matrix3x3::identity(),
            mesh->getDataType());
        mesh->render(renderer->getFrameData());
    }

    if (this->child)
        this->child->render(context);
}

uint UINodeContainer::getWidth()
{
    return width ? width : (child ? child->getWidth() : 0);
}

uint UINodeContainer::getHeight()
{
    return height ? height : (child ? child->getHeight() : 0);
}

void UINodeContainer::setupRendering(Renderer *contextRenderer)
{
    if (renderer != contextRenderer)
    {
        renderer = contextRenderer;
        if (mesh)
            mesh->freeObjectId(objectId);
        mesh = renderer->getDefaultPlane();
        objectId = mesh->genNewObjectId();
    }
}