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
        VulkanText(VulkanUtils *vulkanUtils);
        std::shared_ptr<Texture> getTexture() override final;
        void update() override final;

    protected:
        VulkanUtils *vulkanUtils;
        std::shared_ptr<Texture> texture;
    };
};
