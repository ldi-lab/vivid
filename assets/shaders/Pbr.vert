#version 330 core

// input vertex data
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord0;

// output data
out vec2 vUv;
out vec3 vNormalW;   // normal vector in world space
out vec3 vPosW;      // vertex position in world space

// uniforms
uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat3 normalMatrixW;  // inverse transpose of model rotation matrix

void main() {
    vUv = texCoord0;

    // transform normals from model space to world space
    vNormalW = normalize(normalMatrixW * normal);

    // transform vertex position from model space to world space
    vPosW = vec3(modelMatrix * vec4(position, 1.0));

    gl_Position = MVP * vec4(position, 1.0);
}