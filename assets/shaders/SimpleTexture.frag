#version 330 core

in vec2 vUv;

out vec4 color;

uniform sampler2D myTexture;

void main() {
    color = texture(myTexture, vUv);
    color = vec4(color.x, color.x, color.x, 1.0);
}