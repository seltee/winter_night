#version 450

layout(set = 1, binding = 0) uniform sampler2D albedoTexSampler;
layout(set = 0, binding = 7) uniform sampler2D radianceTexSampler;

layout(location = 0) in vec2 UV;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 worldPosition;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    uint objectId;
    uint lightsAmount;
    uint lightIds[12];
    float normalShadowingFactor;
    float uvShiftX, uvShiftY;
    float uvScaleX, uvScaleY;
} objectData;

layout(set = 0, binding = 3) uniform BufferGlobalData {
     vec4 ambientColor;
     vec4 cameraPosition;
     uint useRadianceMap;
} globalData;

struct LightData
{
    vec4 position;
    vec4 direction;
    vec4 color;
    float affectRadius;
    float cutOff;
    float outerCutOff;
    float fPad3;
    uint shadowTextureId;
    uint amountOfCascades;
    float texelSize;
    uint pad3;
    uint enableDirectional;
    uint enableOmni;
    uint enableSpot;
    uint pad1;
};

layout(set = 0, binding = 4) uniform Lights
{
    LightData lightData[128];
};

layout(set = 0, binding = 6) uniform sampler2D shadowTextures[16];

vec2 sampleSphericalMap(vec3 v);

void main() {
    outColor = texture(albedoTexSampler, UV);
}

vec2 sampleSphericalMap(vec3 v) 
{
    const vec2 invAtan = vec2(0.1591, 0.3183);
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
