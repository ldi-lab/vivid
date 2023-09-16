#version 330 core

in vec2 vUv;

out vec3 color;

uniform sampler2D myTexture;
uniform float time;

void main(){
    color = texture( myTexture, vUv + 0.005*vec2( sin(time+1024.0*vUv.x),cos(time+768.0*vUv.y)) ).xyz ;
}