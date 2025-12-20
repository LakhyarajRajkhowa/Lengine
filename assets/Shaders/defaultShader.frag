#version 330 core

#define MAX_LIGHTS 16

struct Light {
    int type;               // 0 = directional, 1 = point, 2 = spotlight

    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // Attenuation (point & spotlight)
    float constant;
    float linear;
    float quadratic;

    // Spotlight only
    float cutOff;           // cos(inner angle)
    float outerCutOff;      // cos(outer angle)
};

uniform int lightCount;
uniform Light lights[MAX_LIGHTS];

struct Material {
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    vec3 Ke;
    float Ns;

    sampler2D diffuseMap;
    bool hasDiffuseMap;

    sampler2D specularMap;
    bool hasSpecularMap;

    sampler2D normalMap;
    bool hasNormalMap;
    float normalStrength;
};

uniform Material material;

in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;

out vec4 FragColor;

uniform vec3 viewPos;
uniform bool isHovered;

void main()
{
    // ---------------- Diffuse color ----------------
    vec3 diffuseTex = material.Kd;
    if (material.hasDiffuseMap) {
        diffuseTex *= texture(material.diffuseMap, TexCoord).rgb;
    }

    float specMask = 1.0;
    if (material.hasSpecularMap) {
        specMask = texture(material.specularMap, TexCoord).r;
    }

    // ---------------- Normal ----------------
    vec3 normal;
    if (material.hasNormalMap) {
        vec3 normalTex = texture(material.normalMap, TexCoord).rgb;
        normalTex = normalTex * 2.0 - 1.0;
        normalTex.xy *= material.normalStrength;
        normalTex.z = sqrt(1.0 - clamp(dot(normalTex.xy, normalTex.xy), 0.0, 1.0));
        normal = normalize(TBN * normalTex);
    } else {
        normal = normalize(TBN[2]);
    }

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 finalColor = vec3(0.0);

    // ---------------- Lighting loop ----------------
    for (int i = 0; i < lightCount; i++) {

        Light light = lights[i];

        vec3 lightDir;
        float attenuation = 1.0;
        float intensity   = 1.0;

        if (light.type == 0) {
            // Directional light
            lightDir = normalize(-light.direction);
        }
        else {
            // Point or spotlight
            vec3 toLight = light.position - FragPos;
            float distance = length(toLight);
            lightDir = normalize(toLight);

            attenuation = 1.0 / (
                light.constant +
                light.linear * distance +
                light.quadratic * distance * distance
            );

            // Spotlight cone
            if (light.type == 2) {
                float theta = dot(lightDir, normalize(-light.direction));
                float epsilon = light.cutOff - light.outerCutOff;
                intensity = clamp(
                    (theta - light.outerCutOff) / epsilon,
                    0.0,
                    1.0
                );
            }
        }

        // Ambient
        vec3 ambient = light.ambient * diffuseTex;

        // Diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * diffuseTex;

        // Specular
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Ns);
        vec3 specular = light.specular * spec * specMask * material.Ks;

        diffuse  *= attenuation * intensity;
        specular *= attenuation * intensity;

        finalColor += ambient + diffuse + specular;
    }

    if (isHovered) {
        finalColor += vec3(0.0, 0.0, 0.2);
    }

    FragColor = vec4(finalColor, 1.0);
}
