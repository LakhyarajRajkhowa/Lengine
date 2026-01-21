#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 3) in mat4 uModel;   // if instanced

uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vNormalView;

void main()
{
    mat4 model = uModel;

    vec4 viewPos = uView * model * vec4(aPosition, 1.0);
    gl_Position = uProjection * viewPos;

    // Normal → view space
    mat3 normalMatrix = transpose(inverse(mat3(uView * model)));
    vNormalView = normalize(normalMatrix * aNormal);
}
