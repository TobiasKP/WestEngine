#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoord;
layout(location = 2) in vec3 normals;

out vec2 fragTextureCoord;

uniform mat4 transformationMatrix;

layout(binding = 1, std140) uniform cameraUniforms {
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

void main() {
    gl_Position = projectionMatrix * viewMatrix * transformationMatrix * vec4(position, 1.0);  
    fragTextureCoord = textureCoord;
}
