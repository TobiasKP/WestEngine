#version 460 core

#extension GL_EXT_gpu_shader4 : enable

in flat vec4 vColor;
in flat uint vFlags;
in vec2 TexCoord;
in vec2 scale;

out vec4 fragColor;

uniform sampler2D textureSampler;

void main() {
    if ((vFlags & 0x01u) != 0u) {
        float borderWidth = 0.1;
        float maxX = 1.0 - (borderWidth / scale.x);
        float minX = borderWidth / scale.x;
        float maxY = 1.0 - (borderWidth / scale.y);
        float minY = borderWidth / scale.y;
        if (TexCoord.x < maxX && TexCoord.x > minX && TexCoord.y < maxY && TexCoord.y > minY) {
            discard;
        } else {
            fragColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
    } else {
        fragColor = texture(textureSampler, TexCoord);
    }

    if ((vFlags & 0x02u) != 0u) {
        discard;
    }
}
