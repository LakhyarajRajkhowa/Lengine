#version 330 core

in vec3 worldPos;
out vec4 FragColor;

uniform vec3 cameraPos;

// These are authored in sRGB space (because you chose them by eye)
uniform vec3 baseColor  = vec3(0.25, 0.25, 0.25);
uniform vec3 minorColor = vec3(0.3,  0.3,  0.3);
uniform vec3 majorColor = vec3(0.4,  0.4,  0.4);

uniform float gridSpacing = 1.0;
uniform float fadeDistance = 1000.0;

float minorLineThickness = 1.0;
float majorLineThickness = 0.2;

uniform int state;

// ----------------------------------------------------
// Proper sRGB → Linear conversion
// (use real sRGB curve, not pow(2.2) approximation)
// ----------------------------------------------------
vec3 SRGBToLinear(vec3 c)
{
    bvec3 cutoff = lessThanEqual(c, vec3(0.04045));
    vec3 lower = c / 12.92;
    vec3 higher = pow((c + 0.055) / 1.055, vec3(2.4));
    return mix(higher, lower, cutoff);
}

void main()
{
    float minorSpacing = gridSpacing;
    float majorSpacing = gridSpacing * 10.0;

    float lineX_minor = abs(fract(worldPos.x / minorSpacing - 0.5) - 0.5) / (fwidth(worldPos.x) * minorLineThickness);
    float lineZ_minor = abs(fract(worldPos.z / minorSpacing - 0.5) - 0.5) / (fwidth(worldPos.z) * minorLineThickness);
    float line_minor = min(lineX_minor, lineZ_minor);
    float intensity_minor = 1.0 - smoothstep(0.0, 1.0, line_minor);

    float lineX_major = abs(fract(worldPos.x / majorSpacing - 0.5) - 0.5) / (fwidth(worldPos.x) * majorLineThickness);
    float lineZ_major = abs(fract(worldPos.z / majorSpacing - 0.5) - 0.5) / (fwidth(worldPos.z) * majorLineThickness);
    float line_major = min(lineX_major, lineZ_major);
    float intensity_major = 1.0 - smoothstep(0.0, 1.0, line_major);

    float dist = length(cameraPos - worldPos);
    float fade = clamp(1.0 - dist / fadeDistance, 0.0, 1.0);

    // 🔹 Convert authored colors to linear first
    vec3 baseLinear  = SRGBToLinear(baseColor);
    vec3 minorLinear = SRGBToLinear(minorColor);
    vec3 majorLinear = SRGBToLinear(majorColor);

    vec3 color = baseLinear;
    color = mix(color, minorLinear, intensity_minor);
    color = mix(color, majorLinear, intensity_major);

    // Central axes (convert these too!)
    vec3 redLinear  = SRGBToLinear(vec3(1.0, 0.0, 0.0));
    vec3 blueLinear = SRGBToLinear(vec3(0.0, 0.0, 1.0));

    float axisThickness = 0.02;

    float xAxis = smoothstep(0.0, axisThickness, abs(worldPos.z));
    color = mix(redLinear, color, xAxis);

    float zAxis = smoothstep(0.0, axisThickness, abs(worldPos.x));
    color = mix(blueLinear, color, zAxis);

    if (state == 1)
    {
        color += SRGBToLinear(vec3(0.3));
    }

    FragColor = vec4(color * fade, 1.0);
}
