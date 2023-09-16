#version 330 core

// output data
layout(location = 0) out float fragmentDepth;

void main() {
    fragmentDepth = gl_FragCoord.z;
}