#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 finalBonesMatrices[MAX_BONES];


void main()
{
    // ---- SKINNING ----
   mat4 skinMatrix = mat4(1.0); // Identity by default

    bool hasBone = false;
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if(boneIds[i] >= 0 && boneIds[i] < MAX_BONES && weights[i] > 0.0)
        {
            skinMatrix = mat4(0.0);
            hasBone = true;
            break;
        }
    }

    if(hasBone)
    {
        for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {   
            if(boneIds[i] >= 0 && boneIds[i] < MAX_BONES)
                skinMatrix += finalBonesMatrices[boneIds[i]] * weights[i];
        }

        float weightSum = weights.x + weights.y + weights.z + weights.w;
        skinMatrix /= weightSum;
    }

    // Compute final vertex position
    vec4 skinnedPosition = skinMatrix * vec4(aPos, 1.0);

    vec4 worldPos = model * skinnedPosition;

    gl_Position = projection * view * worldPos;

}
