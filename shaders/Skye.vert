#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexture;

// Outputs for the Fragment Shader
out vec3 Normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;

//--Uniforms for user
uniform float waterUpperBound;  // 0.001
uniform float sandUpperBound;   // 0.010
uniform float sandSlopeLimit; // 0.3
uniform float grassSlopeLimit; // 0.683
void main() {
    Normal = aNormal;

    // Base terrain colors
    vec3 waterColor      = vec3(0.0, 0.5, 0.8);  // Blue water
    vec3 sandColor       = vec3(0.9, 0.8, 0.6);  // Beach sand
    vec3 grassColor1     = vec3(0.0, 0.45, 0.15);  // Darker green grass
    vec3 grassColor2     = vec3(0.1, 0.5, 0.2);   // Lush green
    vec3 rockColor1      = vec3(0.4, 0.4, 0.4);   // Darker rock
    vec3 rockColor2      = vec3(0.6, 0.6, 0.6);   // Lighter rock
    vec3 rockColor3      = vec3(0.5, 0.45, 0.4);  // Brownish rock

    // Procedural noise for variation
    float noise = fract(sin(dot(aPos.xz, vec2(12.9898, 78.233))) * 43758.5453);
    noise = mix(-0.08, 0.08, noise); 

    // Compute slope factor (how steep the surface is)
    float slopeFactor = clamp(1.0 - abs(aNormal.y), 0.0, 1.0);

    // Terrain classification based on slope
    if (aPos.y < waterUpperBound) {
        // Water with slight wave-like variation
        float waveNoise = fract(sin(dot(aPos.xz, vec2(37.456, 91.123))) * 43758.5453);
        waveNoise = mix(-0.05, 0.05, waveNoise);
        color = waterColor + vec3(waveNoise * 0.1, waveNoise * 0.1, 0.0);
    } 
    else if (aPos.y < sandUpperBound) {
        if (slopeFactor < sandSlopeLimit) {
            color = sandColor;
        } else {
            // If slope is too steep, mix grass and rocks instead
            float blendFactor = fract(sin(dot(aPos.xz, vec2(55.321, 23.456))) * 43758.5453);
            color = mix(grassColor1, rockColor1, blendFactor);
        }
    }
    else {
        if (slopeFactor < grassSlopeLimit) {
            // Mostly grass on flat areas
            float grassNoise = fract(sin(dot(aPos.xz, vec2(67.123, 13.456))) * 43758.5453);
            color = mix(grassColor1, grassColor2, grassNoise);
        } else {
            // Steeper slopes transition into rocky terrain
            float rockNoise = fract(sin(dot(aPos.xz, vec2(89.654, 45.321))) * 43758.5453);
            if (rockNoise < 0.33) {
                color = rockColor1;
            } else if (rockNoise < 0.66) {
                color = rockColor2;
            } else {
                color = rockColor3;
            }
        }
    }

    // Apply final noise-based variation
    color += vec3(noise * 0.05);

    // Clamp to prevent color artifacts
    color = clamp(color, 0.0, 1.0);

    texCoord = aTexture;
    gl_Position = camMatrix * vec4(aPos, 1.0);
}
