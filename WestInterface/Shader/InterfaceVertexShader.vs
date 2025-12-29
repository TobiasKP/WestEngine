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
    vec2 inversedTexCoords = texCoords / vec2(instanceOffset.z, instanceOffset.w);
    inversedTexCoords = clamp(inversedTexCoords, vec2(0.0), vec2(1.0));
    TexCoord = mix(uvCoords.xy, uvCoords.zw, inversedTexCoords);
    QuadCoord = texCoords;
    scale = vec2(instanceOffset.z, instanceOffset.w);
    float depth = float(gl_InstanceID) * 0.001; // Calculate depth based on gl_InstanceID (sorted by zIndex on the CPU)
    vec2 stretchedPosition = vec2(position.x * instanceOffset.z, position.y * instanceOffset.w);
    gl_Position = orthoMatrix * vec4(stretchedPosition.x + instanceOffset.x, stretchedPosition.y + instanceOffset.y, depth, 1.0);
}
