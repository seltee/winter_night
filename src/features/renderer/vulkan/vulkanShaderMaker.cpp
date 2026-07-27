#include "features/renderer/vulkan/vulkanShaderMaker.h"
#include "features/renderer/vulkan/vulkanShaderSPIRV.h"
#include "features/logger/logger.h"
#include <cstring>

using namespace wne;

extern const char *txtPadding;
extern const char *txtVersion;
extern const char *txtVertexInput;
extern const char *txtOutUV;
extern const char *txtOutNormal;
extern const char *txtOutWorldPosition;
extern const char *txtOutLightClipPos;
extern const char *txtPushConstants;
extern const char *txtSamplerAlbedo;
extern const char *txtSamplerRadiance;
extern const char *txtSamplerShadowTextures;
extern const char *txtMainFunction;
extern const char *txtMainFunctionClose;
extern const char *txtFragmentOutColor;

extern const char *txtFragmentInUV;
extern const char *txtFragmentInNormal;
extern const char *txtFragmentInWorldPosition;
extern const char *txtFragmentInLightClipPos;
extern const char *txtAlbedoAlpha;
extern const char *txtDiscardMaskedAlpha;
extern const char *txtOutColorSolid;

extern const char *txtBufferMVPs;
extern const char *txtBufferMModels;
extern const char *txtBufferMNormals;
extern const char *txtBufferBones;
extern const char *txtBufferGlobalData;
extern const char *txtBufferLights;
extern const char *txtBufferLightMVPs;

extern const char *txtBufferBonesDataStruct;
extern const char *txtBufferLightDataStruct;

extern const char *txtPositionZero;
extern const char *txtPositionVertex;
extern const char *txtNormalZero;
extern const char *txtNormalVertex;

extern const char *txtUVLayout;
extern const char *txtBonesCalculationsMain;
extern const char *txtBonesCalculationsSimple;
extern const char *txtPositionOut;
extern const char *txtNormalOut;

extern const char *txtWorldPosition;

extern const char *txtVertexLightCalculation;

extern const char *txtColorLayout;
extern const char *txtRadiance;
extern const char *txtFragmentLightning;

extern const char *txtColorResultWithLight;
extern const char *txtColorGammaResult;
extern const char *textOutColor;

extern const char *txtFnSphericalMap;

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
    if (stateLightsEnabled)
        vertexMainShaderCode += txtOutLightClipPos;
    vertexMainShaderCode += txtPadding;
    if (stateMaskedEnabled)
    {
        vertexDepthShaderCode += txtOutUV;
        vertexDepthShaderCode += txtPadding;
    }

    // Structures
    if (stateBonesEnabled)
    {
        vertexMainShaderCode += txtBufferBonesDataStruct;
        vertexDepthShaderCode += txtBufferBonesDataStruct;
    }
    if (stateLightsEnabled)
    {
        vertexMainShaderCode += txtBufferLightDataStruct;
        fragmentMainShaderCode += txtBufferLightDataStruct;
    }

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
    if (stateLightsEnabled)
    {
        vertexMainShaderCode += txtBufferLights;
        vertexMainShaderCode += txtBufferLightMVPs;
        fragmentMainShaderCode += txtBufferLights;
        fragmentMainShaderCode += txtBufferLightMVPs;
    }
    fragmentMainShaderCode += txtBufferGlobalData;

    // Fragment Samplers
    fragmentMainShaderCode += txtSamplerAlbedo;
    fragmentMainShaderCode += txtSamplerRadiance;
    fragmentMainShaderCode += txtSamplerShadowTextures;
    if (stateMaskedEnabled)
        fragmentDepthShaderCode += txtSamplerAlbedo;

    // Fragment input
    fragmentMainShaderCode += txtFragmentInUV;
    fragmentMainShaderCode += txtFragmentInNormal;
    fragmentMainShaderCode += txtFragmentInWorldPosition;
    if (stateLightsEnabled)
        fragmentMainShaderCode += txtFragmentInLightClipPos;
    if (stateMaskedEnabled)
        fragmentDepthShaderCode += txtFragmentInUV;

    // Fragment output
    fragmentMainShaderCode += txtFragmentOutColor;
    fragmentDepthShaderCode += txtFragmentOutColor;

    // Functions
    fragmentMainShaderCode += txtFnSphericalMap;

    // ===== Main function =====
    vertexMainShaderCode += std::string(txtPadding) + txtMainFunction;
    vertexDepthShaderCode += std::string(txtPadding) + txtMainFunction;
    fragmentMainShaderCode += std::string(txtPadding) + txtMainFunction;
    fragmentDepthShaderCode += std::string(txtPadding) + txtMainFunction;

    // basic layout
    if (stateBonesEnabled)
    {
        vertexMainShaderCode += txtPositionZero;
        vertexDepthShaderCode += txtPositionZero;
        vertexMainShaderCode += txtNormalZero;
    }
    else
    {
        vertexMainShaderCode += txtPositionVertex;
        vertexDepthShaderCode += txtPositionVertex;
        vertexMainShaderCode += txtNormalVertex;
    }
    vertexMainShaderCode += txtPadding;
    vertexDepthShaderCode += txtPadding;

    fragmentMainShaderCode += txtColorLayout;
    fragmentMainShaderCode += txtRadiance;

    // UV
    vertexMainShaderCode += txtUVLayout;
    if (stateMaskedEnabled)
        vertexDepthShaderCode += txtUVLayout;

    // Bones
    if (stateBonesEnabled)
    {
        vertexMainShaderCode += txtBonesCalculationsMain;
        vertexDepthShaderCode += txtBonesCalculationsSimple;
    }

    // Position and normal output
    vertexMainShaderCode += txtPositionOut;
    vertexDepthShaderCode += txtPositionOut;
    vertexMainShaderCode += txtNormalOut;

    // World position
    vertexMainShaderCode += txtWorldPosition;

    // Lights
    if (stateLightsEnabled)
    {
        vertexMainShaderCode += txtVertexLightCalculation;
        fragmentMainShaderCode += txtFragmentLightning;
    }

    // Fragment output
    if (stateLightsEnabled)
    {
        fragmentMainShaderCode += txtColorResultWithLight;
        fragmentMainShaderCode += txtColorGammaResult;
    }
    fragmentMainShaderCode += textOutColor;

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

    /*
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
    */

    updateNames();
    updateSpirVCache();
}

void VulkanShaderMaker::updateNames()
{
    vertexMainShaderName = "sh";
    vertexDepthShaderName = "sh";
    fragmentMainShaderName = "sh";
    fragmentDepthShaderName = "sh";

    if (stateLightsEnabled)
    {
        vertexMainShaderName += "-lighted";
        vertexDepthShaderName += "-lighted";
        fragmentMainShaderName += "-lighted";
        fragmentDepthShaderName += "-lighted";
    }
    else
    {
        vertexMainShaderName += "-nolight";
        vertexDepthShaderName += "-nolight";
        fragmentMainShaderName += "-nolight";
        fragmentDepthShaderName += "-nolight";
    }

    if (stateBonesEnabled)
    {
        vertexMainShaderName += "-bones";
        vertexDepthShaderName += "-bones";
        fragmentMainShaderName += "-bones";
        fragmentDepthShaderName += "-bones";
    }
    else
    {
        vertexMainShaderName += "-nobones";
        vertexDepthShaderName += "-nobones";
        fragmentMainShaderName += "-nobones";
        fragmentDepthShaderName += "-nobones";
    }

    if (stateMaskedEnabled)
    {
        vertexMainShaderName += "-masked";
        vertexDepthShaderName += "-masked";
        fragmentMainShaderName += "-masked";
        fragmentDepthShaderName += "-masked";
    }
    else
    {
        vertexMainShaderName += "-nomask";
        vertexDepthShaderName += "-nomask";
        fragmentMainShaderName += "-nomask";
        fragmentDepthShaderName += "-nomask";
    }

    vertexMainShaderName += ".vert";
    vertexDepthShaderName += ".vert";
    fragmentMainShaderName += ".frag";
    fragmentDepthShaderName += ".frag";

    vertexMainShaderCachePath = std::string("shader_cache/") + vertexMainShaderName;
    vertexDepthShaderCachePath = std::string("shader_cache/") + vertexDepthShaderName;
    fragmentMainShaderCachePath = std::string("shader_cache/") + fragmentMainShaderName;
    fragmentDepthShaderCachePath = std::string("shader_cache/") + fragmentDepthShaderName;
}

void VulkanShaderMaker::updateSpirVCache()
{
    VulkanShaderSPIRV vertexMainShader(vertexMainShaderCode.c_str(), vertexMainShaderCachePath.c_str(), true);
    if (!vertexMainShader.attemptCompile())
        return;
    vertexMainShaderCompiled.resize(vertexMainShader.getCompCodeLength());
    memcpy(vertexMainShaderCompiled.data(), vertexMainShader.getCompCodeBytes(), vertexMainShader.getCompCodeLength());

    VulkanShaderSPIRV vertexDepthShader(vertexDepthShaderCode.c_str(), vertexDepthShaderCachePath.c_str(), true);
    if (!vertexDepthShader.attemptCompile())
        return;
    vertexDepthShaderCompiled.resize(vertexDepthShader.getCompCodeLength());
    memcpy(vertexDepthShaderCompiled.data(), vertexDepthShader.getCompCodeBytes(), vertexDepthShader.getCompCodeLength());

    VulkanShaderSPIRV fragmentMainShader(fragmentMainShaderCode.c_str(), fragmentMainShaderCachePath.c_str(), false);
    if (!fragmentMainShader.attemptCompile())
        return;
    fragmentMainShaderCompiled.resize(fragmentMainShader.getCompCodeLength());
    memcpy(fragmentMainShaderCompiled.data(), fragmentMainShader.getCompCodeBytes(), fragmentMainShader.getCompCodeLength());

    VulkanShaderSPIRV fragmentDepthShader(fragmentDepthShaderCode.c_str(), fragmentDepthShaderCachePath.c_str(), false);
    if (!fragmentDepthShader.attemptCompile())
        return;
    fragmentDepthShaderCompiled.resize(fragmentDepthShader.getCompCodeLength());
    memcpy(fragmentDepthShaderCompiled.data(), fragmentDepthShader.getCompCodeBytes(), fragmentDepthShader.getCompCodeLength());
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
const char *txtOutLightClipPos = "layout(location = 3) out vec4 outLightClipPos[16];\n";

const char *txtPushConstants =
    "layout(push_constant) uniform PushConstants {              \n\
    uint objectId;                                          \n\
    uint lightsAmount;                                      \n\
    uint lightIds[12];                                      \n\
    float normalShadowingFactor;                            \n\
    float uvShiftX, uvShiftY;                               \n\
    float uvScaleX, uvScaleY;                               \n\
    uint boneIndexesShift, boneMatrixesShift;               \n\
} objectData;                                               \n\n";

const char *txtMainFunction = "void main() {\n";
const char *txtMainFunctionClose = "}\n";

const char *txtFragmentInUV = "layout(location = 0) in vec2 inUV;\n";
const char *txtFragmentInNormal = "layout(location = 1) in vec3 inNormal;\n";
const char *txtFragmentInWorldPosition = "layout(location = 2) in vec4 inWorldPosition;\n";
const char *txtFragmentInLightClipPos = "layout(location = 3) in vec4 inLightClipPos[16];\n";

const char *txtFragmentOutColor = "layout(location = 0) out vec4 outColor;\n";

const char *txtAlbedoAlpha = "float color = texture(albedoTexSampler, UV).a;\n";
const char *txtDiscardMaskedAlpha = "if (color < 0.9) discard;\n";

const char *txtSamplerAlbedo = "layout(set = 1, binding = 0) uniform sampler2D albedoTexSampler;\n";
const char *txtSamplerRadiance = "layout(set = 0, binding = 7) uniform sampler2D radianceTexSampler;\n";
const char *txtSamplerShadowTextures = "layout(set = 0, binding = 6) uniform sampler2D shadowTextures[16];\n";

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

const char *txtBufferLights =
    "layout(set = 0, binding = 4) uniform Lights {      \n\
    LightData lightData[128];                       \n\
};\n";

const char *txtBufferLightMVPs =
    "layout(set = 0, binding = 5) uniform BufferLightMVPs { \n\
    mat4 matrix[16];                                    \n\
} mLightMvps;\n";

const char *txtBufferBones =
    "layout(set = 0, binding = 8, std430) readonly buffer BufferBoneMatrixes {  \n\
    mat4 matrix[4096];                                                      \n\
} mBufferBoneMatrixes;                                                      \n\
layout(set = 0, binding = 9) buffer Bones                                   \n\
{                                                                           \n\
    BoneData boneData[128];                                                 \n\
};\n";

const char *txtBufferGlobalData =
    "layout(set = 0, binding = 3) uniform BufferGlobalData {    \n\
     vec4 ambientColor;                                         \n\
     vec4 cameraPosition;                                       \n\
     uint useRadianceMap;                                       \n\
     float radienceMapFactor;                                   \n\
} globalData;\n";

const char *txtBufferBonesDataStruct =
    "struct BoneData {          \n\
    int boneIndex[5];       \n\
    float boneWeight[5];    \n\
};\n";

const char *txtBufferLightDataStruct =
    "struct LightData {                 \n\
    vec4 position;                  \n\
    vec4 direction;                 \n\
    vec4 color;                     \n\
    float affectRadius;             \n\
    float cutOff;                   \n\
    float outerCutOff;              \n\
    float fPad3;                    \n\
    uint shadowTextureId;           \n\
    uint amountOfCascades;          \n\
    float texelSize;                \n\
    uint pad3;                      \n\
    uint enableDirectional;         \n\
    uint enableOmni;                \n\
    uint enableSpot;                \n\
    uint pad1;                      \n\
};\n\n";

const char *txtPositionZero = "vec4 position = vec4(0.0, 0.0, 0.0, 0.0);\n";
const char *txtPositionVertex = "vec4 position = vec4(inPosition, 1.0);\n";
const char *txtNormalZero = "vec3 normal = vec3(0.0, 0.0, 0.0);\n";
const char *txtNormalVertex = "vec3 normal = (mNormals.matrix[objectData.objectId] * vec4(inNormal, 0.0)).xyz;\n";

const char *txtUVLayout =
    "outUV.x = inUV.x * objectData.uvScaleX + objectData.uvShiftX; \n\
outUV.y = inUV.y * objectData.uvScaleY + objectData.uvShiftY;\n\n";

const char *txtBonesCalculationsMain =
    "for (int i = 0; i < 5; i++) {\n\
    uint boneDataIndex = inVertexID + objectData.boneIndexesShift;\n\
    uint boneIndex = boneData[boneDataIndex].boneIndex[i] + objectData.boneMatrixesShift;\n\
    float boneWeight = boneData[boneDataIndex].boneWeight[i];\n\
    mat4 mWorld = mBufferBoneMatrixes.matrix[boneIndex];\n\
    position += boneWeight * (mWorld * vec4(inPosition, 1.0));\n\
    normal += boneWeight * (mat3(mWorld) * inNormal);\n\
}\n\n";

const char *txtBonesCalculationsSimple =
    "for (int i = 0; i < 5; i++) {\n\
    uint boneDataIndex = inVertexID + objectData.boneIndexesShift;\n\
    uint boneIndex = boneData[boneDataIndex].boneIndex[i] + objectData.boneMatrixesShift;\n\
    float boneWeight = boneData[boneDataIndex].boneWeight[i];\n\
    mat4 mWorld = mBufferBoneMatrixes.matrix[boneIndex];\n\
    position += boneWeight * (mWorld * vec4(inPosition, 1.0));\n\
}\n\n";

const char *txtPositionOut = "gl_Position = mMVPs.matrix[objectData.objectId] * position;\n";
const char *txtNormalOut = "outNormal = normalize(normal);\n";

const char *txtWorldPosition =
    "outWorldPosition = mModels.matrix[objectData.objectId] * position;\n\
outWorldPosition /= outWorldPosition.w;\n\n";

const char *txtVertexLightCalculation =
    "for (uint i = 0; i < objectData.lightsAmount; i++) \n\
{ \n\
    uint id = objectData.lightIds[i]; \n\
    if (lightData[id].enableDirectional != 0) \n\
    { \n\
        if (lightData[id].amountOfCascades > 0) \n\
        { \n\
            uint shadowId = lightData[id].shadowTextureId; \n\
            const float normalOffsetAmount = 0.06; \n\
            vec3 offsetPos = outWorldPosition.xyz + normal * normalOffsetAmount;  // 0.05 – 0.2 often works \n\
            vec4 shadowCoord = mLightMvps.matrix[shadowId] * vec4(offsetPos, 1.0); \n\
            shadowCoord.xyz /= shadowCoord.w; \n\
            shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5; \n\
            outLightClipPos[shadowId] = shadowCoord; \n\
        } \n\
    } \n\
}\n\n";

const char *txtColorLayout =
    "vec4 textureColor = texture(albedoTexSampler, inUV); \n\
vec3 color = textureColor.xyz;                      \n\
float alpha = textureColor.a;                       \n\
vec3 ambientColor = globalData.ambientColor.xyz;\n\n";

const char *txtRadiance =
    "if (globalData.useRadianceMap != 0) {                                              \n\
    vec3 worldDifference = globalData.cameraPosition.xyz - inWorldPosition.xyz;         \n\
    vec3 V = normalize(worldDifference);                                                \n\
    vec2 uv = sampleSphericalMap(inNormal);                                             \n\
    vec2 uv2 = sampleSphericalMap(normalize(reflect(-V, inNormal)));                    \n\
    vec3 irradiance = texture(radianceTexSampler, vec2(uv.x, 1.0 - uv.y)).rgb;          \n\
    ambientColor += irradiance * globalData.radienceMapFactor;                          \n\
};\n\n";

const char *txtFragmentLightning =
    "vec3 light = vec3(0.0, 0.0, 0.0);                                                              \n\
for (uint i = 0; i < objectData.lightsAmount; i++)                                              \n\
{                                                                                               \n\
    uint id = objectData.lightIds[i];                                                           \n\
    if (lightData[id].enableDirectional != 0)                                                   \n\
    {                                                                                           \n\
        vec3 lightDir = lightData[id].direction.xyz;                                            \n\
        vec3 lightColor = lightData[id].color.xyz;                                              \n\
        float diff = max(max(dot(inNormal, lightDir), 0.0), objectData.normalShadowingFactor);    \n\
        float shadow = 0.0f;                                                                    \n\
        if (lightData[id].amountOfCascades > 0){                                                \n\
            float bias = max(0.05 * (1.0 - dot(inNormal, lightDir)), 0.005);                      \n\
            uint shadowId = lightData[id].shadowTextureId;                                      \n\
            const float texelSize = lightData[id].texelSize;                                    \n\
            vec2 projCoords = inLightClipPos[shadowId].xy;                                        \n\
            float currentDepth = inLightClipPos[shadowId].z;                                      \n\
            for (int x = -1; x <= 1; ++x)                                                       \n\
            {                                                                                   \n\
                for (int y = -1; y <= 1; ++y)                                                   \n\
                {                                                                               \n\
                    vec2 coords = (projCoords) + vec2(x, y) * texelSize;                        \n\
                    float pcfDepth = texture(shadowTextures[shadowId], coords).r;               \n\
                    shadow += step(step(currentDepth - bias, pcfDepth) + currentDepth, 1.0);    \n\
                }                                                                               \n\
            }                                                                                   \n\
            if (shadow < 0.2)                                                                   \n\
                shadow = 0;                                                                     \n\
            shadow /= 9.0;                                                                      \n\
        }                                                                                       \n\
        light += diff * lightColor * (1.0f - shadow);                                           \n\
    }                                                                                           \n\
    if (lightData[id].enableOmni != 0)                                                          \n\
    {                                                                                           \n\
        vec3 lightPos = lightData[id].position.xyz;                                             \n\
        vec3 lightColor = lightData[id].color.xyz;                                              \n\
        float affectRadius = lightData[id].affectRadius;                                        \n\
        float dist = length(lightPos - inWorldPosition.xyz);                                      \n\
        float attenuation = clamp(1.0 - dist / affectRadius, 0.0, 1.0);                         \n\
        vec3 lightDir = normalize(lightPos - inWorldPosition.xyz);                                \n\
        float diff = max(max(dot(inNormal, lightDir), 0.0), objectData.normalShadowingFactor);    \n\
        light += diff * lightColor * attenuation;                                               \n\
    }                                                                                           \n\
    if (lightData[id].enableSpot != 0)                                                          \n\
    {                                                                                           \n\
        vec3 lightPos = lightData[id].position.xyz;                                             \n\
        vec3 lightColor = lightData[id].color.xyz;                                              \n\
        float affectRadius = lightData[id].affectRadius;                                        \n\
        float cutOff = lightData[id].cutOff;                                                    \n\
        float outerCutOff = lightData[id].outerCutOff;                                          \n\
        vec3 lightDir = normalize(lightPos - inWorldPosition.xyz);                                \n\
        float theta = dot(lightDir, lightData[id].direction.xyz);                               \n\
        float epsilon = cutOff - outerCutOff;                                                   \n\
        float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);                     \n\
        float dist = length(lightPos - inWorldPosition.xyz);                                      \n\
        float attenuation = clamp(1.0 - dist / affectRadius, 0.0, 1.0);                         \n\
        float diff = max(max(dot(inNormal, lightDir), 0.0), objectData.normalShadowingFactor);    \n\
        light += diff * lightColor * attenuation * intensity;                                   \n\
    }                                                                                           \n\
}\n\n";

const char *txtColorResultWithLight = "color = (light + ambientColor) * color;\n";
const char *txtColorGammaResult = "color = vec3(pow(color.r, 1.0 / 1.2), pow(color.g, 1.0 / 1.2), pow(color.b, 1.0 / 1.2));\n";
const char *textOutColor = "outColor = vec4(color, alpha);\n";

const char *txtFnSphericalMap =
    "vec2 sampleSphericalMap(vec3 v) {                  \n\
    const vec2 invAtan = vec2(0.1591, 0.3183);      \n\
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));      \n\
    uv *= invAtan;                                  \n\
    uv += 0.5;                                      \n\
    return uv;                                      \n\
}\n\n";
