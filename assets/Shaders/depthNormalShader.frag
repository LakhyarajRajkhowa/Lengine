#version 330 core

layout (location = 0) out vec3 gNormal;

in vec3 vNormalView;

void main()
{
    // Store in [-1,1] range (SSAO expects this)
    gNormal = normalize(vNormalView);
}
