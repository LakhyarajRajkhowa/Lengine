#version 330 core

out vec4 FragColor;

uniform vec4 color;
uniform bool isHovered;  
uniform bool isSelected;

void main()
{
    vec3 finalColor = color.rgb;

    if(isHovered){
        finalColor = vec3(1.0, 1.0, 1.0);
    }

    FragColor = vec4(finalColor, 1.0);
}
