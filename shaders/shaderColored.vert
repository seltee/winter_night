#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 normal;

layout(push_constant) uniform PushConstants {
    mat4 mvp;
    mat4 mNormal;
} pushConstants;

void main() {
    gl_Position = pushConstants.mvp * vec4(inPosition, 1.0);
    fragColor = inColor;
    normal = normalize(
        (pushConstants.mNormal * vec4(inNormal, 0.0)).xyz
    );
}