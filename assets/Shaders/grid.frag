#version 330 core

in vec3 worldPos;
out vec4 FragColor;

uniform vec3 cameraPos;

float grid(vec2 coord, float spacing)
{
    vec2 grid = abs(fract(coord / spacing - 0.5) - 0.5) / fwidth(coord / spacing);
    float line = min(grid.x, grid.y);
    return 1.0 - min(line, 1.0);
}

void main()
{
    vec2 coord = worldPos.xz;

    float dist = length(cameraPos.xz - coord);

    float fade = clamp(1.0 - dist / 800.0, 0.0, 1.0);

    float g1 = grid(coord, 1.0);
    float g2 = grid(coord, 10.0);
    float g3 = grid(coord, 100.0);

    vec3 color = vec3(0.15);

    color = mix(color, vec3(0.3), g1);
    color = mix(color, vec3(0.4), g2);
    color = mix(color, vec3(0.5), g3);

    // axis highlight
    float axisWidth = 0.02;

    if(abs(worldPos.x) < axisWidth)
        color = vec3(0,0,1);

    if(abs(worldPos.z) < axisWidth)
        color = vec3(1,0,0);

    FragColor = vec4(color * fade, 1.0);
}