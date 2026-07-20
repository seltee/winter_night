#version 450

layout(location = 0) in uint inVertexID;
layout(location = 1) in vec3 inPosition;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec2 UV;

layout(push_constant) uniform PushConstants {
    uint objectId;
    uint lightsAmount;
    uint lightIds[12];
    float normalShadowingFactor;
    float uvShiftX, uvShiftY;
    float uvScaleX, uvScaleY;
} objectData;

layout(set = 0, binding = 0, std430) readonly buffer BufferMVPs {
     mat4 matrix[8192];
} mMVPs;

void main() {
    gl_Position = mMVPs.matrix[objectData.objectId] * vec4(inPosition, 1.0);
    UV.x = inUV.x * objectData.uvScaleX + objectData.uvShiftX;
    UV.y = inUV.y * objectData.uvScaleY + objectData.uvShiftY;
}