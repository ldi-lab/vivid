#version 330 core

// input values from the vertex shaders
in vec3 vNormal;

// output data
out vec3 color;

void main() {
    vec3 normal = normalize(vNormal);
    float shading = dot(normal, vec3(-0.3, 0.8, 0.6));
    color = vec3(1.0, 0.8, 0.2) + shading * 0.1;
}