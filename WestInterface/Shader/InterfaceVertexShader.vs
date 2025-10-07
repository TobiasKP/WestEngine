#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 iColor;
layout(location = 2) in vec2 instanceOffset;
layout(location = 3) in uint flags;

uniform mat4 orthoMatrix;

out flat vec4 vColor;
out flat uint vFlags;
out vec2 quadPos;

void main() {
    vColor = iColor;
    vFlags = flags;
    quadPos = position;
    gl_Position = orthoMatrix * vec4(position.x + instanceOffset.x, position.y + instanceOffset.y, 0.0, 1.0);
}
