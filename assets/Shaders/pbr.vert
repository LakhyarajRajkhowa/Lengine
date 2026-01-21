
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 WorldPos;
out mat3 TBN;


uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{

    mat3 normalMatrix = transpose(inverse(mat3(model)));

    vec3 T = normalize(mat3(model) * aTangent);
    vec3 B = normalize(mat3(model) * aBitangent);
    vec3 N = normalize(mat3(model) * aNormal);

    TBN = mat3(T, B, N);
    TexCoords = aTexCoords;
    WorldPos = vec3(model * vec4(aPos, 1.0));


   // T = normalize(T - dot(T, N) * N);

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}