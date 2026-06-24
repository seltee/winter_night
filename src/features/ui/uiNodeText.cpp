#include "features/ui/uiNodeText.h"
#include "features/renderer/renderer.h"
#include "features/logger/logger.h"

using namespace wne;

UINodeText::UINodeText(std::shared_ptr<Font> font)
{
    this->font = font;
    this->text = std::make_shared<Text>(font);
}

std::shared_ptr<UINodeText> UINodeText::create(std::shared_ptr<Font> font)
{
    return std::make_shared<UINodeText>(font);
}

std::shared_ptr<UINodeText> UINodeText::create(std::shared_ptr<Font> font, const char *text)
{
    auto textNode = std::make_shared<UINodeText>(font);
    textNode->setText(text);
    return textNode;
}

std::shared_ptr<UINodeText> UINodeText::create(std::shared_ptr<Font> font, const char *text, uint32 fontSize)
{
    auto textNode = std::make_shared<UINodeText>(font);
    textNode->setText(text);
    textNode->setTextSize(fontSize);
    return textNode;
}

std::shared_ptr<UINodeText> UINodeText::create(std::shared_ptr<Font> font, const char *text, uint32 fontSize, uint32 color)
{
    auto textNode = std::make_shared<UINodeText>(font);
    textNode->setText(text);
    textNode->setTextSize(fontSize);
    textNode->setTextColor(color);
    return textNode;
}

UINode::ContextTreeNode UINodeText::update(const ContextUpdate &context)
{
    prepareNewState();
    float texWidth = (float)text->getTextureWidth();
    float texHeight = (float)text->getTextureHeight();

    Matrix4x4 newModel = Matrix4x4::translation(
        (float)context.x + texWidth * 0.5f,
        (float)context.y - texHeight * 0.5f + (float)text->getTextHeight(),
        0);
    newModel = newModel * Matrix4x4::rotationY(PI);
    mModel = newModel * Matrix4x4::scale(Vector3(texWidth, texHeight, 1.0f));

    bool hovered = context.contextGlobal->mouseX >= context.x &&
                   context.contextGlobal->mouseY >= context.y &&
                   context.contextGlobal->mouseX < context.x + (int)text->getTextWidth() &&
                   context.contextGlobal->mouseY < context.y + (int)text->getTextHeight();

    return {hovered};
}

void UINodeText::render(const ContextRender &context)
{
    if (renderer != context.renderer)
    {
        renderer = context.renderer;
        auto newText = renderer->createText(font);
        newText->setFontSize(text->getFontSize());
        newText->setText(text->getText());
        newText->setTextColor(text->getTextColor());
        text = newText;

        if (mesh)
            mesh->freeObjectId(objectId);
        mesh = renderer->getDefaultPlane();
        objectId = mesh->genNewObjectId();

        material = renderer->createFlatMaterial(text->getTexture());
        material->setColorBlending(wne::ColorBlending::Alpha);
        material->setLighted(false);

        isDirtyFlag = false;
    }

    if (isDirtyFlag)
    {
        isDirtyFlag = false;
        text->update();
    }

    if (!renderer)
        return;

    Material *materialToUse = material ? material.get() : renderer->getDefaultMaterial().get();
    if (!materialToUse || objectId == 0xffffffff)
        return;

    auto state = renderer->getState();
    AffectingLights lights{};
    materialToUse->bindColor(objectId, lights, state->getViewProjectionMatrix() * mModel, mModel, Matrix3x3::identity(), mesh->getDataType());
    mesh->render(renderer->getFrameData());
}

uint UINodeText::getWidth()
{
    return text->getTextWidth();
}

uint UINodeText::getHeight()
{
    return text->getTextHeight();
}