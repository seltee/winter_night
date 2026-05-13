#version 450

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 UV;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 fragPos;

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
    vec4 position;
    vec4 direction;
    vec4 color;
    float affectRadius;
    float fPad1;
    float fPad2;
    float fPad3;
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
        if (lightData[id].enableOmni != 0)
        {
            vec3 lightPos = lightData[id].position.xyz;
            vec3 lightColor = lightData[id].color.xyz;
            float affectRadius = lightData[id].affectRadius;

            float dist = length(lightPos - fragPos.xyz);
            float attenuation = clamp(1.0 - dist / affectRadius, 0.0, 1.0);
            vec3 lightDir = normalize(lightPos - fragPos);
            float diff = max(dot(normal, lightDir), 0.0);
            light += diff * lightColor * attenuation;
        }
    }

    vec3 result = light * color;
    outColor = vec4(result, 1.0);

    // outColor = vec4(normal, 1.0);
    // outColor = texture(texSampler, UV);
}
