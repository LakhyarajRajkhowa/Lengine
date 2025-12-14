#version 330 core

struct Material {
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    vec3 Ke;
    float Ns;
};

uniform Material material;

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool isHovered;
void main()
{
    // Base color
    vec3 baseColor = material.Kd;

    // Selection state
     if(isHovered){
        baseColor += vec3(0.0, 0.0, 0.5);
     }

    // Ambient
    vec3 ambient = material.Ka * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * baseColor * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Ns);
    vec3 specular = spec * material.Ks * lightColor;

    vec3 finalColor = ambient + diffuse + specular;
    FragColor = vec4(finalColor, 1.0);
}
