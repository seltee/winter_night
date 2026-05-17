#version 450

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 UV;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 color = texture(texSampler, UV).xyz;
    outColor = vec4(color, 1.0);
}
