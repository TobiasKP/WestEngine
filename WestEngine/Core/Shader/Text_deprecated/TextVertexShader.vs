#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 transformation;
layout(location = 2) in vec2 texCoords0;
layout(location = 3) in vec2 texCoords1;
layout(location = 4) in vec2 texCoords2;
layout(location = 5) in vec2 texCoords3;

uniform mat4 uProjection;

out vec2 fragTextureCoord;

void main() {
    vec2 texCoords;
    if (gl_VertexID % 4 == 0) texCoords = texCoords0;
    else if (gl_VertexID % 4 == 1) texCoords = texCoords1;
    else if (gl_VertexID % 4 == 2) texCoords = texCoords2;
    else texCoords = texCoords3;

    vec3 screenPosition = vec3(position.x + transformation.x, position.y + transformation.y, position.z);
    vec4 pos = uProjection * vec4((screenPosition.x + gl_InstanceID * 150), screenPosition.y, screenPosition.z, 1.0);
    gl_Position = pos;
    fragTextureCoord = texCoords;
}
