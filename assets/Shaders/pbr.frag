#version 330 core
#define MAX_LIGHTS 32


out vec4 FragColor;
out vec4 BrightColor;

in vec2 TexCoords;
in vec3 WorldPos;
in mat3 TBN;

uniform vec3 cameraPos;

uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform int lightCount;

const float PI = 3.14159265359;

// ---------------- PBR functions ----------------

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a  = roughness * roughness;
    float a2 = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    return GeometrySchlickGGX(max(dot(N, V), 0.0), roughness) *
           GeometrySchlickGGX(max(dot(N, L), 0.0), roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);

}

// ---------------- Material ----------------

struct PBRMaterial
{
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;

    sampler2D albedoMap;
    bool hasAlbedoMap;

    sampler2D normalMap;
    bool hasNormalMap;
    float normalStrength;

    sampler2D metallicRoughnessMap;
    bool hasMetallicRoughnessMap;
  
    sampler2D aoMap;
    bool hasAOMap;

    sampler2D metallicMap;
    bool hasMetallicMap;

    sampler2D roughnessMap;
    bool hasRoughnessMap;

};

uniform PBRMaterial material;

// IBL
uniform samplerCube irradianceMap;

vec3 GetNormal()
{
    if (material.hasNormalMap) {
    vec3 mapNormal = texture(material.normalMap, TexCoords).rgb;
    mapNormal = mapNormal * 2.0 - 1.0; // [0,1] → [-1,1]

    mapNormal.xy *= material.normalStrength;
    mapNormal = normalize(mapNormal);

    return normalize(TBN * mapNormal);
    }
     else {
    return normalize(TBN * vec3(0.0, 0.0, 1.0));
    }

}

void main()
{
    vec3 N = GetNormal();
    vec3 V = normalize(cameraPos - WorldPos);

    vec3 albedo = material.albedo;
    float metallic = material.metallic;
    float roughness = material.roughness;
    float ao = material.ao;



    if (material.hasAlbedoMap)
        albedo *= texture(material.albedoMap, TexCoords).rgb;

    if (material.hasMetallicRoughnessMap)
    {
        vec3 orm = texture(material.metallicRoughnessMap, TexCoords).rgb;
        ao        *= orm.r;
        roughness *= orm.g;
        metallic  *= orm.b;
    }
    else
    {
        if (material.hasMetallicMap)
            metallic *= texture(material.metallicMap, TexCoords).r;

        if (material.hasRoughnessMap)
            roughness *= texture(material.roughnessMap, TexCoords).r;

        if (material.hasAOMap)
            ao *= texture(material.aoMap, TexCoords).r;
    }
    roughness = clamp(roughness, 0.04, 1.0);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < lightCount; i++)
    {
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);

        float distance = length(lightPositions[i] - WorldPos);
        vec3 radiance = lightColors[i] / (distance * distance);

        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 specular = (NDF * G * F) /
            max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0), 0.0001);

        vec3 kS = F;
        vec3 kD = (1.0 - kS) * (1.0 - metallic);

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;

    vec3 finalColor = ambient + Lo;

    float alpha = 1.0f;

    // gamma correction
    float gamma = 2.2;
    finalColor = pow(finalColor, vec3(1.0/gamma));

    FragColor = vec4(finalColor, alpha);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
