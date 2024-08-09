#version 330 core

in vec3 FragPos;  
in vec3 Normal;  

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Combine results
    vec3 result = (ambient + 1.5 * diffuse) * vec3(0.0, 0.0, 1.0);
    FragColor = vec4(result, 1.0);
}
