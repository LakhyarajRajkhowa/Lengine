#version 330 core


in vec3 WorldPos;
out vec4 FragColor;

uniform samplerCube environmentMap;
uniform mat3 envRotation;
uniform float envIntensity;
uniform vec3 envTint;

void main()
{
    vec3 dir = normalize(WorldPos);

    // rotate environment
    dir = envRotation * dir;

    vec3 color = texture(environmentMap, dir).rgb;

    // apply tint + intensity
    color *= envTint;
    color *= envIntensity;

    FragColor = vec4(color, 1.0);
}