#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexture;

// Outputs the normal for the Fragment Shader
out vec3 Normal;
// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the Fragment Shader
out vec2 texCoord;

uniform mat4 camMatrix;

void main() {
    Normal = aNormal;
    color = vec3(aPos.y); // Using the height as the color
    texCoord = aTexture;
    gl_Position = camMatrix * vec4(aPos.x, aPos.z, 1.0, 1.0); // 2D projection
}