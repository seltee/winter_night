#version 450

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 UV;
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
    vec3 color = texture(texSampler, UV).xyz;

    // ambient
    vec3 light = globalData.ambientColor.xyz;

    // Diffuse shading
    for (uint i = 0; i < objectData.lightsAmount; i++)
    {
        uint id = objectData.lightIds[i];
        if (lightData[id].enableDirectional != 0)
        {
            vec3 lightDir = lightData[id].direction.xyz;
            vec3 lightColor = lightData[id].color.xyz;
            float diff = max(dot(normal, lightDir), 0.0);
            light += diff * lightColor;
        }
    }

    vec3 result = light * color;
    outColor = vec4(result, 1.0);

    // outColor = vec4(normal, 1.0);
    // outColor = texture(texSampler, UV);
}
