#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBlur;
uniform float exposure;

uniform bool enableBloom;

void main()
{

    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    vec3 bloom = texture(bloomBlur, TexCoords).rgb;

    if(enableBloom)
        hdrColor += bloom; // add bloom

    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    // gamma correction
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}
