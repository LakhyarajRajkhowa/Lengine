#version 330 core

out vec4 FragColor;

uniform vec4 color;
uniform bool isHovered;  
uniform bool isSelected;
void main()
{
    FragColor = vec4(color);
}
