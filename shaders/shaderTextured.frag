#version 450

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 UV;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 worldPosition;
layout(location = 3) in vec4 lightClipPos[16];

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
    float cutOff;
    float outerCutOff;
    float fPad3;
    uint shadowTextureId;
    uint amountOfCascades;
    float texelSize;
    uint pad3;
    uint enableDirectional;
    uint enableOmni;
    uint enableSpot;
    uint pad1;
};

layout(set = 0, binding = 4) uniform Lights
{
    LightData lightData[128];
};

layout(set = 0, binding = 6) uniform sampler2D shadowTextures[16];

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
            float shadow = 0.0f;
            if (lightData[id].amountOfCascades > 0){
                float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); 

                uint shadowId = lightData[id].shadowTextureId;

                const float texelSize = lightData[id].texelSize;
                vec2 projCoords = lightClipPos[shadowId].xy;
                float currentDepth = lightClipPos[shadowId].z;

                for (int x = -1; x <= 1; ++x)
                {
                    for (int y = -1; y <= 1; ++y)
                    {
                        vec2 coords = (projCoords) + vec2(x, y) * texelSize;
                        float pcfDepth = texture(shadowTextures[shadowId], coords).r;
                        shadow += step(step(currentDepth - bias, pcfDepth) + currentDepth, 1.0);
                    }
                }
                if (shadow < 0.2)
                    shadow = 0;
                shadow /= 9.0;
            }

            light += diff * lightColor * (1.0f - shadow);
        }
        if (lightData[id].enableOmni != 0)
        {
            vec3 lightPos = lightData[id].position.xyz;
            vec3 lightColor = lightData[id].color.xyz;
            float affectRadius = lightData[id].affectRadius;

            float dist = length(lightPos - worldPosition.xyz);
            float attenuation = clamp(1.0 - dist / affectRadius, 0.0, 1.0);
            vec3 lightDir = normalize(lightPos - worldPosition.xyz);
            float diff = max(dot(normal, lightDir), 0.0);
            light += diff * lightColor * attenuation;
        }
        if (lightData[id].enableSpot != 0)
        {
            vec3 lightPos = lightData[id].position.xyz;
            vec3 lightColor = lightData[id].color.xyz;
            float affectRadius = lightData[id].affectRadius;
            float cutOff = lightData[id].cutOff;
            float outerCutOff = lightData[id].outerCutOff;

            vec3 lightDir = normalize(lightPos - worldPosition.xyz);
            float theta = dot(lightDir, lightData[id].direction.xyz);

            float epsilon = cutOff - outerCutOff;
            float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

            float dist = length(lightPos - worldPosition.xyz);
            float attenuation = clamp(1.0 - dist / affectRadius, 0.0, 1.0);
            float diff = max(dot(normal, lightDir), 0.0);
            light += diff * lightColor * attenuation * intensity;
        }
    }

    vec3 result = light * color;
    outColor = vec4(result, 1.0);

    // outColor = vec4(normal, 1.0);
    // outColor = texture(texSampler, UV);
}
