#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 UV;

layout(push_constant) uniform PushConstants {
    uint objectId;
    uint lightsAmount;
    uint lightIds[12];
} objectData;

layout(set = 0, binding = 0) uniform BufferMVPs {
     mat4 matrix[256];
} mMVPs;

void main() {
    gl_Position = mMVPs.matrix[objectData.objectId] * vec4(inPosition, 1.0);
    UV = inUV;
}