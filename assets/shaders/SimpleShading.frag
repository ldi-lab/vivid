#version 330 core

// input values from the vertex shaders
in vec2 vUv;
in vec3 vNormal;

// output data
out vec3 color;

// texture
uniform sampler2D diffuseMap;

void main() {
    vec3 light = vec3(0.5, 0.2, 1.0);
    vec3 normal = normalize(vNormal);
    vec3 tex = texture(diffuseMap, vUv).rgb;
    float shading = dot(normal, light) * 0.15;
    color = tex + shading;
}