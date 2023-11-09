#version 330 core

// input values from the vertex shaders
in vec2 vUv;
in vec3 vMVPos; // position in the view space
in vec3 vPos;   // position in the model space

// output data
out vec3 color;

// uniforms
uniform sampler2D diffuseMap;
uniform vec3 uFogColor;
uniform float uFogNear;
uniform float uFogFar;

void main() {
    vec3 tex = texture(diffuseMap, vUv).rgb;

    // add the fog relative to the distance from the camera
    float dist = length(vMVPos);
    float fogAmount = smoothstep(uFogNear, uFogFar, dist);
    color = mix(tex, uFogColor, fogAmount);
}