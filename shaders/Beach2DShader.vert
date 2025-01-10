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
uniform float oceanUpperBound; // 0.01
uniform float lightOceanUpperBound; // 0.02
uniform float sandUpperBound; //  0.03
uniform float lowerGrassUpperBound; // 0.06
uniform float upperGrassUpperBound; // 0.1
uniform float lowerPeaksBound; // 0.12
uniform float upperPeaksBound; // 0.15

void main(){
    Normal = aNormal;

    if (aPos.y < oceanUpperBound) {
        color = vec3(0.2, 0.2, 1.0);
    } else if (aPos.y < lightOceanUpperBound) {
        color = vec3(0.2, 0.4, 1.0);
    } else if (aPos.y < sandUpperBound) {
        color = vec3(1.0, 1.0, 0.8);
    } else if (aPos.y < lowerGrassUpperBound) {
        color = vec3(0.2, 0.6, 0.4);
    } else if (aPos.y < upperGrassUpperBound) {
        color = vec3(0.0, 0.7, 0.5);
    } else if (aPos.y < lowerPeaksBound) {
        color = vec3(0.3, 0.3, 0.3);
    } else if (aPos.y < upperPeaksBound) {
        color = vec3(0.7, 0.7, 0.7);
    } else {
        color = vec3(.95, .95, .95);
    }

    texCoord = aTexture;
    gl_Position = camMatrix * vec4(aPos.x, aPos.z, 1.0, 1.0); // 2D projection
}