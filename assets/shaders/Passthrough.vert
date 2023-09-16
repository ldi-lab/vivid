#version 330 core

layout (location = 0) in vec3 position;

// Output data
out vec2 vUv;

void main() {
    gl_Position = vec4(position, 1);
    vUv = (position.xy + vec2(1, 1) ) / 2;
}