#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 iColor;
layout (location = 2) in vec2 instanceOffset;

uniform mat4 orthoMatrix;

out flat vec4 vColor;

void main() {
    vColor = iColor;
    gl_Position = orthoMatrix * vec4(position.x + instanceOffset.x, position.y + instanceOffset.y, 0.0, 1.0);
}
