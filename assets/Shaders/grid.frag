#version 330 core

in vec3 worldPos;
out vec4 FragColor;

uniform vec3 cameraPos;
uniform vec3 baseColor = vec3(0.25, 0.25,0.25);     // plane color
uniform vec3 minorColor = vec3(0.3, 0.3, 0.3);    // small grid color (1x1)
uniform vec3 majorColor = vec3(0.4, 0.4, 0.4);    // large grid color (10x10)
uniform float gridSpacing = 1.0;  // e.g. 1.0
uniform float fadeDistance = 1000.0; // e.g. 100.0
float minorLineThickness = 1.0;
float majorLineThickness = 0.2;

uniform int state;

void main()
{
    // --------------------------
    // Compute grid line intensity
    // --------------------------
    float minorSpacing = gridSpacing;
    float majorSpacing = gridSpacing * 10.0;

    // Calculate minor grid lines
    float lineX_minor = abs(fract(worldPos.x / minorSpacing - 0.5) - 0.5) / (fwidth(worldPos.x) * minorLineThickness);
    float lineZ_minor = abs(fract(worldPos.z / minorSpacing - 0.5) - 0.5) / (fwidth(worldPos.z) * minorLineThickness);
    float line_minor = min(lineX_minor, lineZ_minor);
    float intensity_minor = 1.0 - smoothstep(0.0, 1.0, line_minor);

    // Calculate major grid lines
    float lineX_major = abs(fract(worldPos.x / majorSpacing - 0.5) - 0.5) / (fwidth(worldPos.x) * majorLineThickness);
    float lineZ_major = abs(fract(worldPos.z / majorSpacing - 0.5) - 0.5) / (fwidth(worldPos.z) *majorLineThickness);
    float line_major = min(lineX_major, lineZ_major);
    float intensity_major = 1.0 - smoothstep(0.0, 1.0, line_major);

    // Fade with distance
    float dist = length(cameraPos - worldPos);
    float fade = clamp(1.0 - dist / fadeDistance, 0.0, 1.0);

    // Combine base + grid layers
    vec3 color = baseColor;
    color = mix(color, minorColor, intensity_minor);
    color = mix(color, majorColor, intensity_major);

    // --------------------------
    // Central colored axes
    // --------------------------
    float axisThickness = 0.02;

    // X-axis (Z=0): Red
    float xAxis = smoothstep(0.0, axisThickness, abs(worldPos.z));
    color = mix(vec3(1.0, 0.0, 0.0), color, xAxis);

    // Z-axis (X=0): Blue
    float zAxis = smoothstep(0.0, axisThickness, abs(worldPos.x));
    color = mix(vec3(0.0, 0.0, 1.0), color, zAxis);

    // selection state
    if(state == 1){
        color.rgb += vec3(0.3);
    }

    FragColor = vec4(color * fade, 1.0);
}
