#include "features/renderer/vulkan/vulkanShaderMaker.h"
#include "features/logger/logger.h"

using namespace wne;

extern const char *txtPadding;
extern const char *txtVersion;
extern const char *txtVertexInput;
extern const char *txtOutUV;
extern const char *txtOutNormal;
extern const char *txtOutWorldPosition;
extern const char *txtPushConstants;
extern const char *txtSamplerAlbedo;
extern const char *txtMainFunction;
extern const char *txtMainFunctionClose;
extern const char *txtFragmentOutColor;
extern const char *txtFragmentInUV;
extern const char *txtNormalInUV;
extern const char *txtWorldPositionInUV;
extern const char *txtAlbedoAlpha;
extern const char *txtDiscardMaskedAlpha;
extern const char *txtOutColorSolid;

extern const char *txtBufferMVPs;
extern const char *txtBufferMModels;
extern const char *txtBufferMNormals;
extern const char *txtBufferBones;

void VulkanShaderMaker::updateShaderCode()
{
    isDirty = false;

    vertexMainShaderCode = "";
    vertexDepthShaderCode = "";
    fragmentMainShaderCode = "";
    fragmentDepthShaderCode = "";

    // Version
    vertexMainShaderCode += txtVersion;
    vertexDepthShaderCode += txtVersion;
    fragmentMainShaderCode += txtVersion;
    fragmentDepthShaderCode += txtVersion;

    // Vertex shader Input
    vertexMainShaderCode += txtVertexInput;
    vertexDepthShaderCode += txtVertexInput;

    // Vertex shader Ouput
    vertexMainShaderCode += txtOutUV;
    vertexMainShaderCode += txtOutNormal;
    vertexMainShaderCode += txtOutWorldPosition;
    vertexMainShaderCode += txtPadding;
    vertexDepthShaderCode += txtOutUV;
    vertexDepthShaderCode += txtPadding;

    // PC
    vertexMainShaderCode += txtPushConstants;
    vertexDepthShaderCode += txtPushConstants;
    fragmentMainShaderCode += txtPushConstants;
    // fragmentDepthShaderCode += txtPushConstants;

    // Buffers
    vertexMainShaderCode += txtBufferMVPs;
    vertexMainShaderCode += txtBufferMModels;
    vertexMainShaderCode += txtBufferMNormals;
    vertexDepthShaderCode += txtBufferMVPs;
    vertexDepthShaderCode += txtBufferMModels;
    if (stateBonesEnabled)
    {
        vertexMainShaderCode += txtBufferBones;
        vertexDepthShaderCode += txtBufferBones;
    }

    // Fragment Samplers
    if (stateMaskedEnabled)
        fragmentDepthShaderCode += txtSamplerAlbedo;

    // Fragment input
    fragmentMainShaderCode += txtFragmentInUV;
    fragmentMainShaderCode += txtNormalInUV;
    fragmentMainShaderCode += txtWorldPositionInUV;
    if (stateMaskedEnabled)
        fragmentDepthShaderCode += txtFragmentInUV;

    // Fragment output
    fragmentMainShaderCode += txtFragmentOutColor;
    fragmentDepthShaderCode += txtFragmentOutColor;

    // Main function
    vertexMainShaderCode += std::string(txtPadding) + txtMainFunction;
    vertexDepthShaderCode += std::string(txtPadding) + txtMainFunction;
    fragmentMainShaderCode += std::string(txtPadding) + txtMainFunction;
    fragmentDepthShaderCode += std::string(txtPadding) + txtMainFunction;

    // Depth function
    if (stateMaskedEnabled)
    {
        fragmentDepthShaderCode += txtAlbedoAlpha;
        fragmentDepthShaderCode += txtDiscardMaskedAlpha;
    }
    fragmentDepthShaderCode += txtOutColorSolid;

    // End of main
    vertexMainShaderCode += txtMainFunctionClose;
    vertexDepthShaderCode += txtMainFunctionClose;
    fragmentMainShaderCode += txtMainFunctionClose;
    fragmentDepthShaderCode += txtMainFunctionClose;

    Logger::log << "Vulkan Shader Maker";
    Logger::log << "Vertex main" << endl;
    Logger::log << vertexMainShaderCode.c_str() << endl
                << endl;
    Logger::log << "Vertex depth" << endl;
    Logger::log << vertexDepthShaderCode.c_str() << endl
                << endl;
    Logger::log << "Fragment main" << endl;
    Logger::log << fragmentMainShaderCode.c_str() << endl
                << endl;
    Logger::log << "Fragment depth" << endl;
    Logger::log << fragmentDepthShaderCode.c_str() << endl
                << endl;
}

const char *txtPadding = "\n";

const char *txtVersion = "#version 450\n\n";

const char *txtVertexInput =
    "layout(location = 0) in uint inVertexID;           \n\
    layout(location = 1) in vec3 inPosition;            \n\
    layout(location = 2) in vec2 inUV;                  \n\
    layout(location = 3) in vec3 inNormal;              \n\n";

const char *txtOutUV = "layout(location = 0) out vec2 outUV;\n";
const char *txtOutNormal = "layout(location = 1) out vec3 outNormal;\n";
const char *txtOutWorldPosition = "layout(location = 2) out vec4 outWorldPosition;\n";

const char *txtPushConstants =
    "layout(push_constant) uniform PushConstants {              \n\
        uint objectId;                                          \n\
        uint lightsAmount;                                      \n\
        uint lightIds[12];                                      \n\
        float uvShiftX, uvShiftY;                               \n\
        float uvScaleX, uvScaleY;                               \n\
        uint boneIndexesShift, boneMatrixesShift;               \n\
    } objectData;                                               \n\n";

const char *txtMainFunction = "void main() {\n";
const char *txtMainFunctionClose = "}\n";

const char *txtFragmentInUV = "layout(location = 0) in vec2 UV;\n";
const char *txtNormalInUV = "layout(location = 1) in vec3 normal;\n";
const char *txtWorldPositionInUV = "layout(location = 2) in vec4 worldPosition;\n";

const char *txtFragmentOutColor = "layout(location = 0) out vec4 outColor;\n";

const char *txtAlbedoAlpha = "float color = texture(albedoTexSampler, UV).a;\n";
const char *txtDiscardMaskedAlpha = "if (color < 0.9) discard;\n";

const char *txtSamplerAlbedo = "layout(set = 1, binding = 0) uniform sampler2D albedoTexSampler;\n";

const char *txtOutColorSolid = "vec4(0.0, 0.0, 0.0, 1.0);\n";

const char *txtBufferMVPs =
    "layout(set = 0, binding = 0, std430) readonly buffer BufferMVPs {      \n\
        mat4 matrix[8192];                                                  \n\
    } mMVPs;\n";

const char *txtBufferMModels =
    "layout(set = 0, binding = 1, std430) readonly buffer BufferObjects {       \n\
    mat4 matrix[8192];                                                      \n\
} mModels;\n";

const char *txtBufferMNormals =
    "layout(set = 0, binding = 2, std430) readonly buffer BufferNormals {       \n\
    mat4 matrix[8192];                                                      \n\
} mNormals;\n";

const char *txtBufferBones =
    "struct BoneData {                                                          \n\
    int boneIndex[5];                                                       \n\
    float boneWeight[5];                                                    \n\
};                                                                          \n\
layout(set = 0, binding = 8, std430) readonly buffer BufferBoneMatrixes {   \n\
     mat4 matrix[4096];                                                     \n\
} mBufferBoneMatrixes;                                                      \n\
layout(set = 0, binding = 9) buffer Bones                                   \n\
{                                                                           \n\
    BoneData boneData[128];                                                 \n\
};";
