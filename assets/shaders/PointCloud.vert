#version 330 core

// input vertex data
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

// output vertex data
out vec3 vColor;

// uniforms
uniform mat4 MVP;
uniform float uPointSize = 1.0;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    gl_PointSize = uPointSize;
    vColor = color;
}