#version 410 core

layout(location = 0) in vec3 position;

layout(std140) uniform cameraUniforms {
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

void main() {
    gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);
}
