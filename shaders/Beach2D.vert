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
uniform float oceanLimit; // 0.01
uniform float lightOceanLimit; // 0.02
uniform float sandLimit; //  0.03
uniform float lowerGrassLimit; // 0.06
uniform float upperGrassLimit; // 0.1
uniform float lowerPeaksLimit; // 0.12
uniform float upperPeaksLimit; // 0.15

void main(){
    Normal = aNormal;

    if (aPos.y < oceanLimit) {
        color = vec3(0.2, 0.2, 1.0);
    } else if (aPos.y < lightOceanLimit) {
        color = vec3(0.2, 0.4, 1.0);
    } else if (aPos.y < sandLimit) {
        color = vec3(1.0, 1.0, 0.8);
    } else if (aPos.y < lowerGrassLimit) {
        color = vec3(0.2, 0.6, 0.4);
    } else if (aPos.y < upperGrassLimit) {
        color = vec3(0.0, 0.7, 0.5);
    } else if (aPos.y < lowerPeaksLimit) {
        color = vec3(0.3, 0.3, 0.3);
    } else if (aPos.y < upperPeaksLimit) {
        color = vec3(0.7, 0.7, 0.7);
    } else {
        color = vec3(.95, .95, .95);
    }

    texCoord = aTexture;
    gl_Position = camMatrix * vec4(aPos.x, aPos.z, 1.0, 1.0); // 2D projection
}