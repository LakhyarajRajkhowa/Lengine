#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;

out vec3 worldPos;

void main()
{
    vec3 pos = aPos;

    pos.xz += cameraPos.xz;

    worldPos = pos;

    gl_Position = projection * view * vec4(pos, 1.0);
}