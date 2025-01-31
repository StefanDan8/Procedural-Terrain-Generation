#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexture;

out vec3 Normal;
out vec3 color;  
out vec2 texCoord;

uniform mat4 camMatrix;

void main() {
    vec3 meshColor = vec3(0.4, 0.4, 0.4); 
    vec3 edgeColor = vec3(0.0, 0.0, 0.0); 
    float edgeThreshold = 0.1; 
    float slopeFactor = clamp(1.0 - abs(aNormal.y), 0.0, 1.0);
    Normal = aNormal;
    texCoord = aTexture;

   color = mix(meshColor, edgeColor, slopeFactor);

    gl_Position = camMatrix * vec4(aPos, 1.0);
}
