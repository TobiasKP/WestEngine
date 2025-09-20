#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 iColor;

uniform mat4 orthoMatrix;

out flat vec4 vColor;

void main() {
    vColor = iColor;
    gl_Position = orthoMatrix * vec4(position.x, position.y, 0.0, 1.0);
}
