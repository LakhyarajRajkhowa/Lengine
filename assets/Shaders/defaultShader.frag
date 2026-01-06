#version 330 core

#define MAX_LIGHTS 16

struct Light {
    int type;               // 0 = directional, 1 = point, 2 = spotlight

    vec3 position;
    vec3 direction;

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

uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

uniform samplerCube shadowCubeMap;
uniform float farPlane;

in vec4 FragPosLightSpace;

uniform vec3 sceneAmbient;
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
uniform bool entityEditingMode;
uniform bool entitySelected;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Outside shadow map
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
    projCoords.y < 0.0 || projCoords.y > 1.0 ||
    projCoords.z > 1.0)
        {
            return 0.0;
        }

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Bias (prevents acne)

    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.002);
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    return shadow;
}
vec3 sampleOffsetDirections[20] = vec3[](
   vec3( 1,  1,  1), vec3(-1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1),
   vec3( 1,  1, -1), vec3(-1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1),
   vec3( 1,  1,  0), vec3(-1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0,  1, -1), vec3( 0, -1, -1)
);
float ShadowCubeMapCalculation(vec3 fragPos, vec3 lightPos)
{
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = max(0.05 * (currentDepth / farPlane), 0.005);

    int samples  = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = 0.05;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= farPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);  

    return shadow;
}
 


void main()
{
    // ---------------- Diffuse color ----------------
    vec3 diffuseTex = material.Kd;
    float texAlpha = 1.0;
    if (material.hasDiffuseMap) {
        vec4 tex = texture(material.diffuseMap, TexCoord);
        diffuseTex *= tex.rgb;
        texAlpha *= tex.a;
    }

    float specMask = 1.0;
    if (material.hasSpecularMap) {
        specMask = texture(material.specularMap, TexCoord).r;
    }

    // ---------------- Normal ----------------
    vec3 normal;
    if (material.hasNormalMap) {
    vec3 mapNormal = texture(material.normalMap, TexCoord).rgb;
    mapNormal = mapNormal * 2.0 - 1.0; // [0,1] → [-1,1]

    // Blend with default tangent normal
    mapNormal.xy *= material.normalStrength;
    mapNormal = normalize(mapNormal);

    normal = normalize(TBN * mapNormal);
    }
     else {
    normal = normalize(TBN * vec3(0.0, 0.0, 1.0));
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
    

        // Diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * diffuseTex;

        // Specular
        vec3 halfwayDir = normalize(lightDir + viewDir);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Ns);
        vec3 specular = light.specular * spec * specMask * material.Ks;

        diffuse  *= attenuation * intensity;
        specular *= attenuation * intensity;

        float shadow = 0.0;

        float theta = dot(lightDir, normalize(-light.direction));

       if (light.type == 0 || light.type == 2) {
        // directional && spotlight
        shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);
    }
        else if (light.type == 1 ) {
        // pointlight
        shadow = ShadowCubeMapCalculation(FragPos, light.position);
    }

        finalColor += (1.0 - shadow) * (diffuse + specular);

    }

    // for individual submesh
    if (isHovered) {
        finalColor += vec3(0.0, 0.0, 0.2);
    }

    // for whole entity
    
    float alpha = texAlpha;
    if (entitySelected && !entityEditingMode) {
        vec3 gold = vec3(0.25, 0.2, 0.075);
        finalColor = mix(finalColor, gold, 0.25);
        finalColor += vec3(0.12, 0.10, 0.02);
        alpha *= 0.7;   
    }


    vec3 ambient = sceneAmbient * diffuseTex;
    finalColor += ambient;
    FragColor = vec4(finalColor, alpha);
}