#version 330 core

// input
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord0;

// output
out vec2 v_uv;

void main()
{
    v_uv = texCoord0;
    gl_Position = vec4(position.xy, 0.0, 1.0);
}