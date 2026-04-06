#version 330 core

layout(location = 0) in vec3 aPos;    // vertex position
layout(location = 1) in vec3 aColor;  // vertex color

out vec3 fragColor;

uniform mat4 u_ViewProj; // combined view-projection matrix

void main()
{
    fragColor = aColor;
    gl_Position = u_ViewProj * vec4(aPos, 1.0);
}