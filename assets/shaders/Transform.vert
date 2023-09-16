#version 330 core

// input
layout(location = 0) in vec3 position;
layout(location = 3) in vec3 color;

// output
out vec3 fragment_color;

// MVP
uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(position, 1);

    // gl_PointSize only applicable for gl.POINTS draw mode
    gl_PointSize = 8.0;

    fragment_color = color;
}
