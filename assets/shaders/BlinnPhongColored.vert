#version 330 core

// Input vertex data
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

// Output data, will be interpolated for each fragment
out vec3 vNormal;          // fragment normal in camera space
out vec3 vPosition;         // fragment position in camera space

// Uniforms
uniform mat4 MVP;
uniform mat3 normalMatrix; // convert normal from model space into camera space
uniform mat4 modelViewMatrix;

void main() {
    // Output position of the vertex, in clip space.
    gl_Position = MVP * vec4(position, 1);

    // Normal in the camera space
    vNormal = normalize(normalMatrix * normal);

    // Position in the camera space
    vPosition = (modelViewMatrix * vec4(position, 1)).xyz;
}