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

//--Uniforms for user
uniform float lavaLimit; // 0.01
uniform float lightlavaLimit; // 0.02
uniform float lowerStoneLimit; // 0.06
uniform float upperStoneLimit; // 0.1
uniform float lowerAshLimit; // 0.12
uniform float upperAshLimit; // 0.2

void main(){
    Normal = aNormal;

    if (aPos.y < lavaLimit) {
        color = vec3(0.8, 0.3, 0.1);
    } else if (aPos.y < lightlavaLimit) {
        color = vec3(0.9, 0.4, 0.2);
    } else if (aPos.y < lowerStoneLimit) {
        color = vec3(0.2, 0.2, 0.2);
    } else if (aPos.y < upperStoneLimit) {
        color = vec3(0.3, 0.3, 0.3);
    } else if (aPos.y < lowerAshLimit) {
        color = vec3(0.35, 0.35, 0.3);
    } else if (aPos.y < upperAshLimit) {
        color = vec3(0.55, 0.55, 0.5);
    } else {
        color = vec3(.95, .95, .95);
    }

    texCoord = aTexture;
    gl_Position = camMatrix * vec4(aPos.x, aPos.z, 1.0, 1.0); // 2D projection
}