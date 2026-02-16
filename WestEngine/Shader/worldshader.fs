#version 460 core

in vec3 worldPos;
in vec2 QuadCoord;

out vec4 fragColor;

uniform vec3 ddColor;
uniform uint tileFlags[512];
uniform int gridSize;

vec3 burgundy = vec3(0.557, 0.231, 0.275);

void calculateBorderEffect() {
    float borderWidth = 0.05;
    float maxX = 1.0 - borderWidth;
    float minX = borderWidth;
    float maxY = 1.0 - borderWidth;
    float minY = borderWidth;
    if (QuadCoord.x < maxX && QuadCoord.x > minX && QuadCoord.y < maxY && QuadCoord.y > minY) {
        fragColor = vec4(ddColor, 1.0);
    } else {
        fragColor = vec4(burgundy.rgb, 1.0);
    }
}

void main() {
    int index = int(floor(worldPos.z)) * int(gridSize) + int(floor(worldPos.x));
    if ((tileFlags[index] & 0x0001u) != 0u) {
        calculateBorderEffect();
    } else {
        fragColor = vec4(ddColor, 1.0);
    }
}
