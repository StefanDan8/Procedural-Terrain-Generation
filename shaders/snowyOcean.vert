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
uniform float lavaUpperBound; // 0.001 (height for lava rivers)
uniform float ashUpperBound;  // 0.002 (height for ashy ground)
uniform float rockUpperBound; // 0.511 (height for rocky terrain)
uniform float magmaCrackThreshold; // 0.676 (slope threshold for magma cracks)

void main() {
    Normal = aNormal;
    vec3 lavaColor = vec3(1.0, 0.3, 0.1); // Bright orange-red for lava
    vec3 magmaColor = vec3(0.8, 0.2, 0.0); // Darker red for magma
    vec3 ashColor = vec3(0.2, 0.2, 0.2); // Gray for ashy ground
    vec3 rockColor = vec3(0.3, 0.3, 0.3); // Dark gray for rocks
    vec3 crackColor = vec3(0.9, 0.4, 0.1); // Glowing cracks for magma

    // Procedural noise for variation
    float noise = fract(sin(dot(aPos.xz, vec2(12.9898, 78.233))) * 43758.5453);
    noise = mix(-0.1, 0.1, noise); // Add subtle variation

    // Slope factor for blending
    float slopeFactor = clamp(1.0 - abs(aNormal.y), 0.0, 1.0);

    // Height-based color blending
    if (aPos.y < lavaUpperBound) {
        // Lava rivers
        float lavaNoise = fract(sin(dot(aPos.xz, vec2(37.456, 91.123))) * 43758.5453);
        lavaNoise = mix(-0.1, 0.1, lavaNoise);
        color = lavaColor + vec3(lavaNoise * 0.2, lavaNoise * 0.1, 0.0);
    } else if (aPos.y < ashUpperBound) {
        // Ashy ground with magma cracks
        float blendFactor = smoothstep(lavaUpperBound, ashUpperBound, aPos.y);
        color = mix(magmaColor, ashColor, blendFactor);

        // Add glowing cracks on steep slopes
        if (slopeFactor > magmaCrackThreshold) {
            float crackIntensity = smoothstep(magmaCrackThreshold, 1.0, slopeFactor);
            color = mix(color, crackColor, crackIntensity * 0.8);
        }
    } else if (aPos.y < rockUpperBound) {
        // Rocky terrain
        float blendFactor = smoothstep(ashUpperBound, rockUpperBound, aPos.y);
        color = mix(ashColor, rockColor, blendFactor);

        // Add subtle glowing cracks
        if (slopeFactor > magmaCrackThreshold) {
            float crackIntensity = smoothstep(magmaCrackThreshold, 1.0, slopeFactor);
            color = mix(color, crackColor, crackIntensity * 0.5);
        }
    } else {
        // Dark, rugged peaks
        color = rockColor + vec3(noise * 0.05);
    }

    // Add glowing effect to lava and cracks
    if (aPos.y < lavaUpperBound || slopeFactor > magmaCrackThreshold) {
        float glow = sin(aPos.x * 10.0 + aPos.z * 10.0) * 0.5 + 0.5;
        color += vec3(glow * 0.2, glow * 0.1, 0.0);
    }

    // Clamp color to avoid artifacts
    color = clamp(color, 0.0, 1.0);

    // Pass through texture coordinates
    texCoord = aTexture;

    // Transform vertex position
    gl_Position = camMatrix * vec4(aPos, 1.0);
}