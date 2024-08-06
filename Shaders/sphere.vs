#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // Add normal attribute
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 FragPos;  // To pass the position to the fragment shader
out vec3 Normal;   // To pass the normal to the fragment shader

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0f)); // Transform the vertex position
    Normal = mat3(transpose(inverse(model))) * aNormal; // Transform the normal correctly

    gl_Position = projection * view * model * vec4(aPos, 1.0f); 
}
