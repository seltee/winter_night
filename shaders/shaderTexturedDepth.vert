#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 UV;

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

/*
layout(set = 0, binding = 8, std430) readonly buffer BufferBoneMatrixes {
     mat4 matrix[4096];
} mBufferBoneMatrixes;

struct BoneData
{
    int boneIndex[5];
    float boneWeight[5];
};

layout(set = 0, binding = 9) uniform Bones
{
    BoneData boneData[128];
};
*/

void main() {
    vec4 position = mMVPs.matrix[objectData.objectId] * vec4(inPosition, 1.0);
    uint vertexID = gl_VertexIndex;
    
    UV.x = inUV.x * objectData.uvScaleX + objectData.uvShiftX;
    UV.y = inUV.y * objectData.uvScaleY + objectData.uvShiftY;

    /*
    if (objectData.enableBones != 0)
    {
        for (int i = 0; i < 5; i++)
        {
            uint boneDataIndex = vertexID + objectData.boneIndexesShift;
            uint boneIndex = boneData[boneDataIndex].boneIndex[i];
            float boneWeight = boneData[boneDataIndex].boneIndex[i];

            mat4 mWorld = mBufferBoneMatrixes.matrix[objectData.boneMatrixesShift + boneIndex];

            position += boneWeight * mWorld * vec4(inPosition, 1.0);
        }
    }
    */

    gl_Position = position;
}