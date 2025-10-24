#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 iColor;
layout(location = 2) in vec4 instanceOffset;
layout(location = 3) in vec2 texCoords;
layout(location = 4) in vec4 uvCoords;
layout(location = 5) in uint flags;

uniform mat4 orthoMatrix;

out flat vec4 vColor;
out flat uint vFlags;
out vec2 TexCoord;
out vec2 QuadCoord;
out vec2 scale;

void main() {
    vColor = iColor;
    vFlags = flags;
    TexCoord = mix(uvCoords.xy, uvCoords.zw, texCoords);
    QuadCoord = texCoords;
    scale = vec2(instanceOffset.z, instanceOffset.w);
    gl_Position = orthoMatrix * vec4((position.x + instanceOffset.x) * instanceOffset.z, (position.y + instanceOffset.y) * instanceOffset.w, 0.0, 1.0);
}
