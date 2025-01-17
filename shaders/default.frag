#version 330 core

out vec4 FragColor;

// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

void main(){
    FragColor = vec4(color, 1.0f); 
}