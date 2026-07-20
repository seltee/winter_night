#include "features/renderer/vulkan/pipelines/vulkanPipelineTextured.h"
#include "features/renderer/vulkan/vulkanShader.h"
#include "features/renderer/vulkan/vulkanRenderPass.h"
#include "features/renderer/vulkan/vulkanDescriptorLayout.h"
#include "features/renderer/vulkan/materials/vulkanMaterial.h"
#include "features/renderer/vulkan/vulkanDefines.h"
#include "features/logger/logger.h"
#include "core/data.h"
#include "vulkan/vulkan.h"
#include <vector>
#include <array>

using namespace wne;

VulkanPipelineTextured::VulkanPipelineTextured(VulkanDevice *vulkanDevice) : VulkanPipeline(vulkanDevice)
{
}

VulkanPipelineTextured::~VulkanPipelineTextured()
{
    auto device = vulkanDevice->getDevice();
    if (graphicsPipeline)
    {
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        graphicsPipeline = nullptr;
    }
    if (pipelineLayout)
    {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        pipelineLayout = nullptr;
    }
}

VkPipeline VulkanPipelineTextured::getGraphicsPipeline()
{
    return graphicsPipeline;
}

VkPipelineLayout VulkanPipelineTextured::getPipelineLayout()
{
    return pipelineLayout;
}

bool VulkanPipelineTextured::setupParametred(VulkanRenderPass *renderPass, bool enableLights, ColorBlending blending, uint64 MSAASmapleCountBit)
{
    if (!(enableLights ? buildShaderColor() : buildShaderColorNoLights()))
    {
        Logger::log << "Unable to build shader" << endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedColor())
    {
        Logger::log << "Unable to setup textured color pipeline" << endl;
        return false;
    }

    descriptorSetLayoutSampler = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutSampler->setupSampler())
    {
        Logger::log << "Unable to setup textured color pipeline" << endl;
        return false;
    }

    // if solid then it's solid stage and depth is prepared so op is equal
    if (!buildPipeline(2, true, false, true, true, true, false, blending == ColorBlending::Solid, MSAASmapleCountBit, blending, renderPass))
    {
        Logger::log << "Unable to build pipeline" << endl;
        return false;
    }

    return true;
}

bool VulkanPipelineTextured::setupDepth(VulkanRenderPass *depthPass, uint64 MSAASmapleCountBit)
{
    if (!buildShaderDepth())
    {
        Logger::log << "Unable to build shader" << endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedDepth())
    {
        Logger::log << "Unable to setup textured depth pipeline" << endl;
        return false;
    }

    // mask needs fragment shader otherwise only vertex depth needed
    if (!buildPipeline(1, false, true, true, false, true, false, false, MSAASmapleCountBit, ColorBlending::Solid, depthPass))
    {
        Logger::log << "Unable to build pipeline" << endl;
        return false;
    }

    return true;
}

bool VulkanPipelineTextured::setupMaskedDepth(VulkanRenderPass *depthPass, uint64 MSAASmapleCountBit)
{
    if (!buildShaderMaskedDepth())
    {
        Logger::log << "Unable to build shader" << endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedDepth())
    {
        Logger::log << "Unable to setup textured depth pipeline" << endl;
        return false;
    }

    descriptorSetLayoutSampler = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutSampler->setupSampler())
    {
        Logger::log << "Unable to setup textured color pipeline" << endl;
        return false;
    }

    // mask needs fragment shader otherwise only vertex depth needed
    if (!buildPipeline(2, false, true, true, true, true, false, false, MSAASmapleCountBit, ColorBlending::Solid, depthPass))
    {
        Logger::log << "Unable to build pipeline" << endl;
        return false;
    }

    return true;
}

bool VulkanPipelineTextured::setupDepthShadow(VulkanRenderPass *depthPass, bool isDoubleSided)
{
    if (!buildShaderDepth())
    {
        Logger::log << "Unable to build shader" << endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedDepth())
    {
        Logger::log << "Unable to setup textured depth pipeline" << endl;
        return false;
    }

    // mask needs fragment shader otherwise only vertex depth needed
    if (!buildPipeline(1, false, true, true, false, !isDoubleSided, false, false, 1, ColorBlending::Solid, depthPass))
    {
        Logger::log << "Unable to build pipeline" << endl;
        return false;
    }

    return true;
}

bool VulkanPipelineTextured::setupMaskedDepthShadow(VulkanRenderPass *depthPass, bool isDoubleSided)
{
    if (!buildShaderMaskedDepth())
    {
        Logger::log << "Unable to build shader" << endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedDepth())
    {
        Logger::log << "Unable to setup textured depth pipeline" << endl;
        return false;
    }

    descriptorSetLayoutSampler = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutSampler->setupSampler())
    {
        Logger::log << "Unable to setup textured color pipeline" << endl;
        return false;
    }

    // mask needs fragment shader otherwise only vertex depth needed
    if (!buildPipeline(2, false, true, true, true, !isDoubleSided, false, false, 1, ColorBlending::Solid, depthPass))
    {
        Logger::log << "Unable to build pipeline" << endl;
        return false;
    }

    return true;
}

bool VulkanPipelineTextured::setupAtmosphere(VulkanRenderPass *renderPass, uint64 MSAASmapleCountBit)
{
    if (!buildShaderAtmosphere())
    {
        Logger::log << "Unable to build shader" << endl;
        return false;
    }

    descriptorSetLayoutPipeline = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutPipeline->setupTexturedColor())
    {
        Logger::log << "Unable to setup textured color pipeline" << endl;
        return false;
    }

    descriptorSetLayoutSampler = std::make_unique<VulkanDescriptorSetLayout>(vulkanDevice);
    if (!descriptorSetLayoutSampler->setupSampler())
    {
        Logger::log << "Unable to setup textured color pipeline" << endl;
        return false;
    }

    if (!buildPipeline(2, true, false, false, true, true, true, false, MSAASmapleCountBit, ColorBlending::Solid, renderPass))
    {
        Logger::log << "Unable to build pipeline" << endl;
        return false;
    }

    return true;
}

VulkanDescriptorSetLayout *VulkanPipelineTextured::getDescriptorSetLayoutPipeline()
{
    return descriptorSetLayoutPipeline.get();
}

VulkanDescriptorSetLayout *VulkanPipelineTextured::getDescriptorSetLayoutSampler()
{
    return descriptorSetLayoutSampler.get();
}

bool VulkanPipelineTextured::buildShaderColor()
{
    auto device = vulkanDevice->getDevice();
    shader = std::make_unique<VulkanShader>();
    if (!shader->makeFromFiles("./shaders/shaderTextured.vert.spv", "./shaders/shaderTextured.frag.spv", device))
    {
        Logger::log << "Unable to compile textured shader" << endl;
        return false;
    }
    return true;
}

bool VulkanPipelineTextured::buildShaderColorNoLights()
{
    auto device = vulkanDevice->getDevice();
    shader = std::make_unique<VulkanShader>();
    if (!shader->makeFromFiles("./shaders/shaderTexturedNoLight.vert.spv", "./shaders/shaderTexturedNoLight.frag.spv", device))
    {
        Logger::log << "Unable to compile textured shader" << endl;
        return false;
    }
    return true;
}

bool VulkanPipelineTextured::buildShaderDepth()
{
    auto device = vulkanDevice->getDevice();
    shader = std::make_unique<VulkanShader>();
    if (!shader->makeFromFiles("./shaders/shaderTexturedDepth.vert.spv", "./shaders/shaderTexturedDepth.frag.spv", device))
    {
        Logger::log << "Unable to compile textured shader" << endl;
        return false;
    }
    return true;
}

bool VulkanPipelineTextured::buildShaderMaskedDepth()
{
    auto device = vulkanDevice->getDevice();
    shader = std::make_unique<VulkanShader>();
    if (!shader->makeFromFiles("./shaders/shaderTexturedDepth.vert.spv", "./shaders/shaderTexturedMaskedDepth.frag.spv", device))
    {
        Logger::log << "Unable to compile textured shader" << endl;
        return false;
    }
    return true;
}

bool VulkanPipelineTextured::buildShaderAtmosphere()
{
    auto device = vulkanDevice->getDevice();
    shader = std::make_unique<VulkanShader>();
    if (!shader->makeFromFiles("./shaders/shaderAtmosphere.vert.spv", "./shaders/shaderAtmosphere.frag.spv", device))
    {
        Logger::log << "Unable to compile textured shader" << endl;
        return false;
    }
    return true;
}

bool VulkanPipelineTextured::buildPipeline(
    uint32 stageAmount,
    bool enableColorBlending,
    bool enableDepthWrite,
    bool enableDepthTest,
    bool enableSampler,
    bool faceCooling,
    bool reverseFaceCooling,
    bool opEqual,
    uint64 MSAASmapleCountBit,
    ColorBlending blending,
    VulkanRenderPass *renderPass)
{
    auto device = vulkanDevice->getDevice();

    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(VertexTextured);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributeDescriptions[4];
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32_UINT;
    attributeDescriptions[0].offset = offsetof(VertexTextured, inVertexID);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(VertexTextured, pos);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(VertexTextured, uv);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(VertexTextured, normal);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 4;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkExtent2D extent = {512, 512};
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = faceCooling ? (reverseFaceCooling ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_BACK_BIT) : VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f;          // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = (VkSampleCountFlagBits)MSAASmapleCountBit;
    multisampling.minSampleShading = 1.0f;          // Optional
    multisampling.pSampleMask = nullptr;            // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE;      // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    if (enableColorBlending)
    {
        // ColorBlending blending
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = (blending == ColorBlending::Solid) ? VK_FALSE : VK_TRUE;
        if (blending == ColorBlending::Solid)
        {
        }
        else if (blending == ColorBlending::Addition)
        {
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }
        else if (blending == ColorBlending::Substraction)
        {
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_SUBTRACT;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_SUBTRACT;
        }
        else if (blending == ColorBlending::Alpha)
        {
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }

        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional
    }
    else
    {
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.attachmentCount = 0;
        colorBlending.pAttachments = nullptr;
    }

    VkPushConstantRange pushRange{};
    pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushRange.offset = 0;
    pushRange.size = sizeof(PushConstantObject);

    if (enableSampler)
    {
        VkDescriptorSetLayout layouts[2] = {
            descriptorSetLayoutPipeline->getDescriptorSetLayout(),
            descriptorSetLayoutSampler->getDescriptorSetLayout()};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 2;
        pipelineLayoutInfo.pSetLayouts = layouts;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushRange;

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            Logger::log << "unable to create pipeline layout" << endl;
            return false;
        }
    }
    else
    {
        VkDescriptorSetLayout layouts[1] = {descriptorSetLayoutPipeline->getDescriptorSetLayout()};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = layouts;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushRange;

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            Logger::log << "unable to create pipeline layout" << endl;
            return false;
        }
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = enableDepthTest ? VK_TRUE : VK_FALSE;
    depthStencil.depthWriteEnable = enableDepthWrite ? VK_TRUE : VK_FALSE;
    depthStencil.depthCompareOp = opEqual ? VK_COMPARE_OP_EQUAL : VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = stageAmount;
    pipelineInfo.pStages = (VkPipelineShaderStageCreateInfo *)shader->getShaderStages();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass->getRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    vkDeviceWaitIdle(device);

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
    {
        Logger::log << "Unable to create graphics pipeline" << endl;
        return false;
    }
    if (graphicsPipeline == VK_NULL_HANDLE)
        Logger::log << "Pipeline is null!" << endl;

    return true;
}
