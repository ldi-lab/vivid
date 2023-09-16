#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;


// vShadowCoord is in clip space, so vShadowCoord.xyz / vShadowCoord.z
// ranges from (-1, -1, -1) to (1, 1, 1)
out vec4 vShadowCoord;

// Values that stay constant for the whole mesh.
uniform mat4 modelMatrix;
uniform mat4 MVP;

// shadowCameraProjection * shadowCameraView
uniform mat4 shadowCameraPV;


void main(){

    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(position, 1);

    vShadowCoord = shadowCameraPV * modelMatrix * vec4(position, 1);
}