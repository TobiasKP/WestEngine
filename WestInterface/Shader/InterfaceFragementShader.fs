#version 460 core

#extension GL_EXT_gpu_shader4 : enable

in flat vec4 vColor;
in flat uint vFlags;
in vec2 TexCoord;
in vec2 QuadCoord;
in vec2 scale;

out vec4 fragColor;

uniform sampler2D fontTextureSampler;
uniform sampler2DArray textureSampler;

vec4 sampleTextureWithTransparency() {
    vec4 texColor = texture(fontTextureSampler, TexCoord);

    float whiteness = (texColor.r + texColor.g + texColor.b) / 3.0;
    if (whiteness > 0.99) {
        discard;
    }

    return texColor;
}

void calculateBorderEffect(vec4 color, bool dis) {
    float borderWidth = 0.1;
    float maxX = 1.0 - (borderWidth / scale.x);
    float minX = borderWidth / scale.x;
    float maxY = 1.0 - (borderWidth / scale.y);
    float minY = borderWidth / scale.y;
    if (QuadCoord.x < maxX && QuadCoord.x > minX && QuadCoord.y < maxY && QuadCoord.y > minY) {
        if (dis) {
            discard;
        } else {
            fragColor = sampleTextureWithTransparency();
        }
    } else {
        fragColor = color;
    }
}

void calculateGlowEffect(vec3 glowColor, float intensity) {
    vec4 baseColor = sampleTextureWithTransparency();
    fragColor = vec4(baseColor.rgb + (glowColor * intensity), baseColor.a);
}

void main() {
    if ((vFlags & 0x01u) != 0u) {
        calculateBorderEffect(vec4(0.0, 0.0, 0.0, 1.0), true);
    } else {
        fragColor = sampleTextureWithTransparency();
    }

    if ((vFlags & 0x02u) != 0u) {
        calculateGlowEffect(vec3(1.0, 0.5, 0.0), 0.3);
    }
}
