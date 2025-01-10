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
uniform float sandUpperBound; //  0.04
uniform float grassUpperBound; // 0.6
uniform float normalForestUpper; // 0.6
uniform float normalGrassLower; // 0.35


void main(){
    Normal = aNormal;
    vec3 oceanColor = vec3(0.0, 0.3, 0.7);
    vec3 sandColor = vec3(0.9, 0.8, 0.6); 
    vec3 lightGreenGrass = vec3(0.0, 0.72, 0.0);
    vec3 darkGreen1 = vec3(0.165, 0.471, 0.165);
    vec3 darkGreen2 = vec3(0.047, 0.31, 0.047);
    vec3 rockColor = vec3(0.7, 0.7, 0.7); 
    vec3 darkerRockColor = vec3(0.3, 0.3, 0.3);
    // Determine color based on the height (aPos.y)

    float slopeFactor = clamp(1.0 - abs(aNormal.y), 0.0, 1.0);
    if (aPos.y >= -1.0 && aPos.y < oceanUpperBound) {
        float variation = fract(sin(dot(aPos.xz, vec2(12.9898, 78.233))) * 43758.5453); // Simple pseudo-random noise
        variation = mix(-0.02, 0.02, variation); 
        color = oceanColor + variation;
        color = clamp(color, 0.0, 1.0); 
        
      
    } else if (aPos.y >= oceanUpperBound && aPos.y < sandUpperBound && slopeFactor<0.3) {
        float variation = fract(sin(dot(aPos.xz, vec2(12.9898, 78.233))) * 43758.5453); // Simple pseudo-random noise
        variation = mix(-0.02, 0.02, variation); 
        float blendFactor = smoothstep(oceanUpperBound - 0.01, oceanUpperBound + 0.01, aPos.y);
        color = mix(oceanColor, sandColor, blendFactor)+variation;
        color = clamp(color, 0.0, 1.0); 
    } else if (aPos.y >= sandUpperBound && aPos.y < grassUpperBound){
        if(aNormal.y > normalGrassLower){
            if(aNormal.y < normalForestUpper){
                color = mix(darkGreen1, darkGreen2, slopeFactor);
            }else{
                color = mix(darkGreen1,lightGreenGrass, slopeFactor);
            }
        }
        else{
            color = mix(darkerRockColor, rockColor, slopeFactor);
        } 
     
    }else{
            color = mix(darkerRockColor, rockColor, slopeFactor);
        } 
    texCoord = aTexture;
    gl_Position = camMatrix * vec4(aPos, 1.0);
}