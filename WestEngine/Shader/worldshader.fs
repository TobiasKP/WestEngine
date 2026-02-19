#version 460 core

in vec3 worldPos;
in vec2 QuadCoord;

out vec4 fragColor;

uniform vec3 ddColor;
uniform uint tileFlags[512];
uniform int gridSize;

vec3 burgundy = vec3(0.557, 0.231, 0.275);
vec3 gold = vec3(0.851, 0.643, 0.255);
int index = -1;
float borderWidth = 0.05;

bool isNeighbour(int idx) {
    if ((tileFlags[idx] & 0x0002u) != 0) {
        return true;
    }
    return false;
}

void getNeighbours(int row, int col, vec3 color) {
    bool n = false;
    if (QuadCoord.x < borderWidth && col != 0) {
        n = n || isNeighbour(index - 1);
    }
    if (QuadCoord.x > 1.0 - borderWidth && col < gridSize - 1) {
        n = n || isNeighbour(index + 1);
    }
    if (QuadCoord.y < borderWidth && row != 0) {
        n = n || isNeighbour(index - gridSize);
    }
    if (QuadCoord.y > 1.0 - borderWidth && row < gridSize - 1) {
        n = n || isNeighbour(index + gridSize);
    }
    if (n) {
        fragColor = vec4(ddColor.rgb, 1.0);
    } else {
        fragColor = vec4(color.rgb, 1.0);
    }
}

void calculateBorderEffect(vec3 color, bool indicator) {
    float maxX = 1.0 - borderWidth;
    float minX = borderWidth;
    float maxY = 1.0 - borderWidth;
    float minY = borderWidth;
    if (QuadCoord.x < maxX && QuadCoord.x > minX && QuadCoord.y < maxY && QuadCoord.y > minY) {
        fragColor = vec4(ddColor, 1.0);
    } else {
        if (indicator) {
            int row = int(floor(worldPos.z));
            int col = int(floor(worldPos.x));
            getNeighbours(row, col, color);
        } else {
            fragColor = vec4(color.rgb, 1.0);
        }
    }
}

void main() {
    index = int(floor(worldPos.z)) * int(gridSize) + int(floor(worldPos.x));
    if ((tileFlags[index] & 0x0001u) != 0u) {
        calculateBorderEffect(burgundy, false);
    } else if ((tileFlags[index] & 0x0002u) != 0) {
        calculateBorderEffect(gold, true);
    } else {
        fragColor = vec4(ddColor, 1.0);
    }
}
