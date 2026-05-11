#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 normal;
layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightDir = normalize(-vec3(1.0, -2.0, 0.8));
    vec3 ambient = vec3(0.2, 0.2, 0.2);
    vec3 lightColor = vec3(2.0, 2.0, 2.0);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * fragColor;

    outColor = vec4(result, 1.0);
}