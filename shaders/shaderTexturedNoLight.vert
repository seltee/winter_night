#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 UV;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec4 worldPosition;

layout(push_constant) uniform PushConstants {
    uint objectId;
    uint lightsAmount;
    uint lightIds[12];
} objectData;

layout(set = 0, binding = 0) uniform BufferMVPs {
     mat4 matrix[8192];
} mMVPs;

layout(set = 0, binding = 1) uniform BufferObjects {
     mat4 matrix[8192];
} mModels;

layout(set = 0, binding = 2) uniform BufferNormals {
     mat4 matrix[8192];
} mNormals;

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

layout(set = 0, binding = 5) uniform BufferLightMVPs {
     mat4 matrix[16];
} mLightMvps;

void main() {
    gl_Position = mMVPs.matrix[objectData.objectId] * vec4(inPosition, 1.0);
    UV = inUV;
    worldPosition = mModels.matrix[objectData.objectId] * vec4(inPosition, 1.0);
    worldPosition /= worldPosition.w;

    normal = normalize(
        (mNormals.matrix[objectData.objectId] * vec4(inNormal, 0.0)).xyz
    );
}