#pragma once
#include "features/renderer/text.h"
#include "features/renderer/texture.h"
#include "core/api.h"

namespace wne
{
    class VulkanUtils;

    class WNE_API VulkanText : public Text
    {
    public:
        VulkanText(std::shared_ptr<Font> font, VulkanUtils *vulkanUtils);
        static std::shared_ptr<wne::VulkanText> create(std::shared_ptr<Font> font, VulkanUtils *vulkanUtils);

        std::shared_ptr<Texture> getTexture() override final;
        void update() override final;
        float getProportion() override final;

    protected:
        VulkanUtils *vulkanUtils;
        std::shared_ptr<Texture> texture;
        float proportion = 1.0f;
    };
};
