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
uniform float waterUpperBound; // 0.006 
uniform float sandUpperBound; // 0.010
uniform float grassUpperBound; // 0.286 
uniform float rockUpperBound;  // 0.626 
uniform float snowLowerBound;  // 0.722

void main() {
    Normal = aNormal;
    vec3 waterColor = vec3(0.0, 0.5, 0.8); 
    vec3 sandColor = vec3(0.9, 0.8, 0.6);  
    vec3 grassColor = vec3(0.0, 0.6, 0.2); 
    vec3 forestColor = vec3(0.0, 0.4, 0.1); 
    vec3 rockColor = vec3(0.5, 0.5, 0.5);  
    vec3 snowColor = vec3(1.0, 1.0, 1.0); 

    // Procedural noise for variation
    float noise = fract(sin(dot(aPos.xz, vec2(12.9898, 78.233))) * 43758.5453);
    noise = mix(-0.1, 0.1, noise); 

    float slopeFactor = clamp(1.0 - abs(aNormal.y), 0.0, 1.0);

    if (aPos.y < waterUpperBound) {
        float waveNoise = fract(sin(dot(aPos.xz, vec2(37.456, 91.123))) * 43758.5453);
        waveNoise = mix(-0.05, 0.05, waveNoise);
        color = waterColor + vec3(waveNoise * 0.1, waveNoise * 0.1, 0.0);
    } else if (aPos.y < sandUpperBound) {
        //float blendFactor = smoothstep(waterUpperBound, grassUpperBound, aPos.y);
       // color = mix(sandColor, grassColor, blendFactor);
        if(slopeFactor < 0.4){
            color = sandColor;
        }else{
            color = grassColor;
        }

        if (aNormal.y > 0.8) {
            float forestNoise = fract(sin(dot(aPos.xz, vec2(23.456, 67.890))) * 43758.5453);
            forestNoise = mix(0.0, 0.3, forestNoise);
            color = mix(color, forestColor, forestNoise);
        }
    }else if (aPos.y < grassUpperBound){
        color = grassColor;
    } else if (aPos.y < rockUpperBound) {
        //float blendFactor = smoothstep(grassUpperBound, rockUpperBound, aPos.y);
        color = rockColor;

        if (aPos.y > snowLowerBound - 0.1) {
            float snowNoise = fract(sin(dot(aPos.xz, vec2(45.678, 12.345))) * 43758.5453);
            snowNoise = mix(0.0, 0.5, snowNoise);
            color = mix(color, snowColor, snowNoise);
        }
    } else {
        color = snowColor;
    }

    if (slopeFactor > 0.6) {
        color = mix(color, rockColor * 0.7, slopeFactor);
    }

    // Add procedural variation
    color += vec3(noise * 0.05);

    // Clamp color to avoid artifacts
    color = clamp(color, 0.0, 1.0);

    texCoord = aTexture;

    gl_Position = camMatrix * vec4(aPos, 1.0);
}