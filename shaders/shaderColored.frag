#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 normal;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    uint objectId;
    uint lightsAmount;
    uint lightIds[12];
} objectData;

layout(set = 0, binding = 3) uniform BufferGlobalData {
     vec4 ambientColor;
} globalData;

struct LightData
{
    vec4 direction;
    vec4 color;
    uint enableDirectional;
    uint enableOmni;
    uint enableSpot;
    uint pad1;
};

layout(set = 0, binding = 4) uniform Lights
{
    LightData lightData[128];
};

void main() {
    vec3 color = fragColor;

    // ambient
    vec3 light = globalData.ambientColor.xyz;

    // Diffuse shading
    for (uint i = 0; i < objectData.lightsAmount; i++)
    {
        if (lightData[i].enableDirectional != 0)
        {
            vec3 lightDir = lightData[i].direction.xyz;
            vec3 lightColor = lightData[i].color.xyz;
            float diff = max(dot(normal, lightDir), 0.0);
            light += diff * lightColor;
        }
    }

    vec3 result = light * color;
    outColor = vec4(result, 1.0);
}