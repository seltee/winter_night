#version 450
layout(set = 1, binding = 0) uniform sampler2D albedoTexSampler;

layout(location = 0) in vec2 UV;

layout(location = 0) out vec4 outColor;

void main() {
    float color = texture(albedoTexSampler, UV).a;
    if (color < 0.9)
        discard;
    outColor = vec4(0.0, 0.0, 0.0, 1.0);
}
