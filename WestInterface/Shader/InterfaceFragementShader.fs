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

bool handled = false;
float blendingWidth = 0.1;

vec4 sampleText() {
    vec4 texColor = texture(fontTextureSampler, TexCoord);

    float whiteness = (texColor.r + texColor.g + texColor.b) / 3.0;
    if (whiteness > 0.99) {
        discard;
    }

    float darkness = 1.0 - whiteness;
    return vec4(vColor.rgb, darkness);
}

void blending(vec2 coords, float z, float zn) {
    vec2 fracCoords = vec2(fract(coords.x), coords.y);
    vec4 texA = texture(textureSampler, vec3(fracCoords, z));
    if ((vFlags & 0x0020u) != 0u) {
        float cubeCount = scale.x * 0.5;
        float cubePos = fract(TexCoord.x * cubeCount);
        if (cubePos > (1.0 - blendingWidth) || cubePos < blendingWidth) {
            vec4 texB = texture(textureSampler, vec3(fracCoords, zn));
            float normalizedPos = (cubePos - (1.0 - blendingWidth)) / blendingWidth;
            float blendFactor = smoothstep(0.0, 1.0, normalizedPos);
            fragColor = mix(texA, texB, blendFactor);
        } else {
            fragColor = texA;
        }
    } else {
        fragColor = texA;
    }
}

void textureCheck() {
    if ((vFlags & 0x04u) != 0u) {
        float cubeCount = scale.x * 0.5;
        vec2 repeatedCoord = vec2(TexCoord.x * cubeCount, TexCoord.y);
        int virtualCube = int(floor(TexCoord.x * cubeCount));
        float z = float(virtualCube % 2);
        float zn = float((virtualCube + 1) % 2);
        blending(repeatedCoord, z, zn);
    } else {
        discard;
    }
}

void calculateBorderEffect(vec4 color) {
    float borderWidth = 0.1;
    float maxX = 1.0 - (borderWidth / scale.x);
    float minX = borderWidth / scale.x;
    float maxY = 1.0 - (borderWidth / scale.y);
    float minY = borderWidth / scale.y;
    if (QuadCoord.x < maxX && QuadCoord.x > minX && QuadCoord.y < maxY && QuadCoord.y > minY && !handled) {
        textureCheck();
    } else {
        fragColor = color;
    }
}

void calculateGlowEffect(vec3 glowColor, float intensity) {
    vec4 baseColor = sampleText();
    float baseBrightness = dot(baseColor.rgb, vec3(0.299, 0.587, 0.114));
    float adjustedIntensity = baseBrightness > 0.7 ? intensity * 0.3 : intensity;
    vec3 effectColor = mix(baseColor.rgb, glowColor, adjustedIntensity);
    fragColor = vec4(effectColor, 1.0);
}

void main() {
    if ((vFlags & 0x01u) != 0u) {
        calculateBorderEffect(vec4(0.0, 0.0, 0.0, 1.0));
        handled = true;
    }

    if ((vFlags & 0x02u) != 0u) {
        calculateGlowEffect(vec3(1.0, 0.5, 0.0), 0.3);
        handled = true;
    } else if ((vFlags & 0x08u) != 0u) {
        fragColor = sampleText();
        handled = true;
    }

    if (!handled) {
        textureCheck();
    }
}
