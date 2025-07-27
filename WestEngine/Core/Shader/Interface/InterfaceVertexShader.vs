#version 460 core

layout(location = 0) in vec2 root_position;
layout(location = 1) in vec2 screen_position;

out vec2 fragTextureCoord;

layout(location = 0) uniform mat4 m_ortho;

void main() {
    vec2 position = vec2(root_position.xy + screen_position);
    gl_Position = m_ortho * vec4(position.x, position.y, 1.0, 1.0);
}
