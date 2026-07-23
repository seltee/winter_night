#version 450

layout(set = 1, binding = 0) uniform sampler2D albedoTexSampler;
layout(set = 0, binding = 7) uniform sampler2D radianceTexSampler;

layout(location = 0) in vec2 UV;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 worldPosition;
layout(location = 3) in vec4 lightClipPos[16];

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    uint objectId;
    uint lightsAmount;
    uint lightIds[12];
    float normalShadowingFactor;
    float uvShiftX, uvShiftY;
    float uvScaleX, uvScaleY;
} objectData;

layout(set = 0, binding = 3) uniform BufferGlobalData {
     vec4 ambientColor;
     vec4 cameraPosition;
     uint useRadianceMap;
     float radienceMapFactor;
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

vec2 sampleSphericalMap(vec3 v);

void main() {
    vec4 textureColor = texture(albedoTexSampler, UV);
    vec3 color = textureColor.xyz;

    // ambient + radiance
    vec3 ambientColor = globalData.ambientColor.xyz;
    if (globalData.useRadianceMap != 0)
    {
        vec3 worldDifference = globalData.cameraPosition.xyz - worldPosition.xyz;
        vec3 V = normalize(worldDifference);

        vec2 uv = sampleSphericalMap(normal);
        vec2 uv2 = sampleSphericalMap(normalize(reflect(-V, normal)));
        vec3 irradiance = texture(radianceTexSampler, vec2(uv.x, 1.0 - uv.y)).rgb;
        ambientColor += irradiance * globalData.radienceMapFactor;
    }

    // Diffuse light shading
    vec3 light = vec3(0.0, 0.0, 0.0);
    for (uint i = 0; i < objectData.lightsAmount; i++)
    {
        uint id = objectData.lightIds[i];
        if (lightData[id].enableDirectional != 0)
        {
            vec3 lightDir = lightData[id].direction.xyz;
            vec3 lightColor = lightData[id].color.xyz;

            float diff = max(max(dot(normal, lightDir), 0.0), objectData.normalShadowingFactor);
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
            float diff = max(max(dot(normal, lightDir), 0.0), objectData.normalShadowingFactor);
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
            float diff = max(max(dot(normal, lightDir), 0.0), objectData.normalShadowingFactor);
            light += diff * lightColor * attenuation * intensity;
        }
    }

    vec3 result = (light + ambientColor) * color;
    vec3 gammaResult = vec3(pow(result.r, 1.0 / 1.2), pow(result.g, 1.0 / 1.2), pow(result.b, 1.0 / 1.2));
    outColor = vec4(gammaResult, textureColor.a);
}

vec2 sampleSphericalMap(vec3 v) 
{
    const vec2 invAtan = vec2(0.1591, 0.3183);
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
