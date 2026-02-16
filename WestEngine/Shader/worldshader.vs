#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoord;

layout(binding = 1, std140) uniform cameraUniforms {
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

out vec2 QuadCoord;
out vec3 worldPos;

void main() {
    QuadCoord = textureCoord;
    worldPos = position;
    gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);
}
