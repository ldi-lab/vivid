#version 330 core

// Input vertex data
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord0;

// Output data
out vec2 vUv;
out vec3 vNormal;   // normal vector in camera space

// Uniforms
uniform mat4 MVP;
uniform mat3 normalMatrix;  // inverse transpose of modelview matrix, for transforming normals from object space to camera space


void main() {
    vUv = texCoord0;
    vNormal = normalize(normalMatrix * normal);
    gl_Position = MVP * vec4(position, 1.0);
}