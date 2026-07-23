#version 450

layout(location = 0) in uint inVertexID;
layout(location = 1) in vec3 inPosition;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec2 UV;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 worldPosition;
layout(location = 3) out vec4 lightClipPos[16];

layout(push_constant) uniform PushConstants {
    uint objectId;
    uint lightsAmount;
    uint lightIds[12];
    float normalShadowingFactor;
    float uvShiftX, uvShiftY;
    float uvScaleX, uvScaleY;
    uint enableBones, boneIndexesShift, boneMatrixesShift;
} objectData;

layout(set = 0, binding = 0, std430) readonly buffer BufferMVPs {
     mat4 matrix[8192];
} mMVPs;

layout(set = 0, binding = 1, std430) readonly buffer BufferObjects {
     mat4 matrix[8192];
} mModels;

layout(set = 0, binding = 2, std430) readonly buffer BufferNormals {
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

layout(set = 0, binding = 8, std430) readonly buffer BufferBoneMatrixes {
     mat4 matrix[4096];
} mBufferBoneMatrixes;

struct BoneData
{
    int boneIndex[5];
    float boneWeight[5];
};

layout(set = 0, binding = 9) buffer Bones
{
    BoneData boneData[128];
};

void main() {
    vec4 position = vec4(0.0, 0.0, 0.0, 0.0);
    vec3 normal = vec3(0.0, 0.0, 0.0);

    UV.x = inUV.x * objectData.uvScaleX + objectData.uvShiftX;
    UV.y = inUV.y * objectData.uvScaleY + objectData.uvShiftY;

    if (objectData.enableBones != 0)
    {
        for (int i = 0; i < 5; i++)
        {
            uint boneDataIndex = inVertexID + objectData.boneIndexesShift;
            uint boneIndex = boneData[boneDataIndex].boneIndex[i] + objectData.boneMatrixesShift;
            float boneWeight = boneData[boneDataIndex].boneWeight[i];

            mat4 mWorld = mBufferBoneMatrixes.matrix[boneIndex];

            position += boneWeight * (mWorld * vec4(inPosition, 1.0));
            normal += boneWeight * (mat3(mWorld) * inNormal);
        }
    } else {
        position = vec4(inPosition, 1.0);
        normal = (mNormals.matrix[objectData.objectId] * vec4(inNormal, 0.0)).xyz;
    }
    
    gl_Position = mMVPs.matrix[objectData.objectId] * position;
    outNormal = normalize(normal);

    worldPosition = mModels.matrix[objectData.objectId] * position;
    worldPosition /= worldPosition.w;

    for (uint i = 0; i < objectData.lightsAmount; i++)
    {
        uint id = objectData.lightIds[i];
        if (lightData[id].enableDirectional != 0)
        {
            if (lightData[id].amountOfCascades > 0)
            {
                uint shadowId = lightData[id].shadowTextureId;
                const float normalOffsetAmount = 0.06;
                vec3 offsetPos = worldPosition.xyz + normal * normalOffsetAmount;  // 0.05 – 0.2 often works

                vec4 shadowCoord = mLightMvps.matrix[shadowId] * vec4(offsetPos, 1.0);
                shadowCoord.xyz /= shadowCoord.w;
                shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;

                lightClipPos[shadowId] = shadowCoord;
            }
        }
    }
}