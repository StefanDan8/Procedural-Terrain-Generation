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
uniform float sandLowerBound; //  0.01
uniform float sandUpperBound; //  0.04
uniform float grassLowerBound; // 0.04
uniform float grassUpperBound; // 0.1


void main(){
    Normal = aNormal;
    // Determine color based on the height (aPos.y)
    if (aPos.y >= -1.0 && aPos.y < oceanUpperBound) {
        // Ocean blue
        color = vec3(0.0, 0.3, 0.7); // Example blue
    } else if (aPos.y >= sandLowerBound && aPos.y < sandUpperBound) {
        // Sand color
        color = vec3(0.9, 0.8, 0.6); // Example sand
    } else if (aPos.y >= grassLowerBound && aPos.y < grassUpperBound && aNormal.y > 0.55){
        color = vec3(0.24, 0.70, 0.44); // Example grass
    } else {
        // Rocky gray
        color = vec3(0.5, 0.5, 0.5); // Example gray
    }
    texCoord = aTexture;
    gl_Position = camMatrix * vec4(aPos, 1.0);
}