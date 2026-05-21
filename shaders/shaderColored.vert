#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 position;

layout(push_constant) uniform PushConstants {
    uint objectId;
    uint lightsAmount;
    uint lightIds[12];
} objectData;

layout(set = 0, binding = 0) uniform BufferObjects {
     mat4 matrix[8192];
} mModels;

layout(set = 0, binding = 1) uniform BufferMVPs {
     mat4 matrix[8192];
} mMVPs;

layout(set = 0, binding = 2) uniform BufferNormals {
     mat4 matrix[8192];
} mNormals;


void main() {
    position = inPosition;
    gl_Position = mMVPs.matrix[objectData.objectId] * vec4(inPosition, 1.0);
    fragColor = inColor;
    normal = normalize(
        (mNormals.matrix[objectData.objectId] * vec4(inNormal, 0.0)).xyz
    );
}