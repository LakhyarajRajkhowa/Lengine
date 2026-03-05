#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in mat3 TBN;

uniform int u_DebugMode;

// material inputs (reuse same ones!)
uniform vec3 albedoColor;
uniform sampler2D albedoMap;
uniform bool hasAlbedoMap;

uniform sampler2D normalMap;
uniform bool hasNormalMap;
uniform float normalStrength;

float nearPlane = 0.1f;
float farPlane = 25.0f;
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * nearPlane * farPlane) /
           (farPlane + nearPlane - z * (farPlane - nearPlane));
}

vec3 GetDebugNormal()
{
    if (hasNormalMap)
    {
        vec3 n = texture(normalMap, TexCoords).rgb;
        n = n * 2.0 - 1.0;
        n.xy *= normalStrength;
        return normalize(TBN * n);
    }
    else
    {
        return normalize(TBN[2]);
    }
}



void main()
{
    // 0️⃣ Geometry view (just show geometry)
    if (u_DebugMode == 0)
    {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // 1️⃣ Albedo view
    if (u_DebugMode == 1)
    {
               
        vec3 albedo = albedoColor;
        if (hasAlbedoMap)
            albedo *= texture(albedoMap, TexCoords).rgb;

        vec3 finalColor = vec3(albedo);

        FragColor = vec4(finalColor, 1.0f);
        return;
    }

    // 2️⃣ Normal view (world-space)
    if (u_DebugMode == 2)
    {
        vec3 n = GetDebugNormal();
        FragColor = vec4(n * 0.5 + 0.5, 1.0);
        return;
    }

    // 3️⃣ Depth view
    if (u_DebugMode == 3)
    {
        float depth = LinearizeDepth(gl_FragCoord.z) / farPlane;
        FragColor = vec4(vec3(depth), 1.0);
        return;
    }

    FragColor = vec4(1, 0, 1, 1); // error pink
}
