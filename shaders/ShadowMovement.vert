#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexture;

out vec3 Normal;
out vec3 color;  
out vec2 texCoord;

uniform mat4 camMatrix;
//--Uniforms for user
uniform float edgeThreshold; // 0.1
uniform float intensity; // 0.4 

void main() {
    vec3 meshColor = vec3(intensity); 
    vec3 edgeColor = vec3(0.0, 0.0, 0.0); 

    Normal = aNormal;
    texCoord = aTexture;

    vec3 viewDir = normalize(- vec3(camMatrix * vec4(aPos, 1.0)));

    float diffuse = max(dot(Normal, viewDir), 0.0);

    if (diffuse < edgeThreshold) {
        color = edgeColor; 
    } else {
        color = meshColor;
    }

    gl_Position = camMatrix * vec4(aPos, 1.0);
}
