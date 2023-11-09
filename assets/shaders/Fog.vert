#version 330 core

// Input vertex data
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord0;

// Output data
out vec2 vUv;
out vec3 vMVPos;   // position in the view space
out vec3 vPos;     // position in the model space

// Uniforms
uniform mat4 MVP;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main() {
    vUv = texCoord0;
    vPos = position;

    // pass model view position to fragment shader to get distance from camera
    vMVPos = (modelViewMatrix * vec4(position, 1.0)).xyz;

    gl_Position = projectionMatrix * vec4(vMVPos, 1.0);
}