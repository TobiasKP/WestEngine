#version 410 core

layout(location = 0) in vec3 position;
layout(std140) uniform cameraUniforms {
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

out vec3 direction;

void main() {
    direction = vec3(position.x, position.y * 1.75, position.z);
    vec4 pos = projectionMatrix * mat4(mat3(viewMatrix)) * vec4(position, 1.0);
    gl_Position = pos.xyww;
}
