#version 460 core

#extension GL_EXT_gpu_shader4 : enable

in flat vec4 vColor;
in flat uint vFlags;
in vec2 TexCoord;
in vec2 QuadCoord;
in vec2 scale;

out vec4 fragColor;

uniform sampler2D fontTextureSampler;
uniform sampler2D textureSampler;

bool handled = false;
vec3 burgundy = vec3(0.557, 0.231, 0.275);
vec3 gold = vec3(0.851, 0.643, 0.255);
vec3 emerald = vec3(0.251, 0.506, 0.278);

vec4 discardWhitePixels(vec4 texColor) {
    float whiteness = (texColor.r + texColor.g + texColor.b) / 3.0;
    if (whiteness > 0.89) {
        discard;
    }

    float darkness = 1.0 - whiteness;
    return vec4(vColor.rgb, darkness);
}

vec4 sampleText() {
    vec4 texColor = texture(fontTextureSampler, TexCoord);
    return discardWhitePixels(texColor);
}

void textureCheck() {
    if ((vFlags & 0x04u) != 0u) {
        vec4 texColor = texture(textureSampler, TexCoord);
        fragColor = discardWhitePixels(texColor);
    } else if ((vFlags & 0x0010u) != 0u) {
        discard;
    } else {
        fragColor = vColor;
    }
}

void calculateBorderEffect() {
    float borderWidth = 0.05;
    float maxX = 1.0 - (borderWidth / scale.x);
    float minX = borderWidth / scale.x;
    float maxY = 1.0 - (borderWidth / scale.y);
    float minY = borderWidth / scale.y;
    if (QuadCoord.x < maxX && QuadCoord.x > minX && QuadCoord.y < maxY && QuadCoord.y > minY && !handled) {
        textureCheck();
    } else {
        fragColor = vec4(vColor.rgb, 1.0);
    }
}

void calculateGlowEffect(float intensity) {
    vec3 glowColor;
    if ((vFlags & 0x0040u) != 0u) {
        glowColor = burgundy;
    } else if ((vFlags & 0x0080u) != 0u) {
        glowColor = gold;
    } else if ((vFlags & 0x0100u) != 0u) {
        glowColor = emerald;
    } else {
        glowColor = vec3(1.0, 0.0, 0.0);
    }
    vec4 baseColor = vec4(1.0, 0.0, 0.0, 1.0);
    if ((vFlags & 0x0008u) != 0u) {
        baseColor = sampleText();
    } else {
        textureCheck();
    }
    float baseBrightness = dot(baseColor.rgb, vec3(0.299, 0.587, 0.114));
    float adjustedIntensity = baseBrightness > 0.7 ? intensity * 0.3 : intensity;
    vec3 effectColor = mix(baseColor.rgb, glowColor, adjustedIntensity);
    fragColor = vec4(effectColor, 1.0);
}

void main() {
    if ((vFlags & 0x0001u) != 0u || (vFlags & 0x0010u) != 0u) {
        calculateBorderEffect();
        handled = true;
    }

    if ((vFlags & 0x0002u) != 0u) {
        calculateGlowEffect(2.0);
        handled = true;
    } else if ((vFlags & 0x0008u) != 0u) {
        fragColor = sampleText();
        handled = true;
    }

    if (!handled) {
        textureCheck();
    }
}
