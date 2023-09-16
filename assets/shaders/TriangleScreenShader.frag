#version 330 core

in vec2 v_uv;

out vec3 color;

uniform float uTime;
uniform vec3 uColor;

void main() {
    color = vec3(0.3, 0.3, 0.3) + 0.3 * cos(v_uv.xyx + uTime) + uColor;
}