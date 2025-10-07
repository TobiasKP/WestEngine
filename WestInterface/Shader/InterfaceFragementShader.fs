#version 460 core

in flat vec4 vColor;
in flat uint vFlags;
in vec2 quadPos;

out vec4 fragColor;

void main() {
    if ((vFlags & 0x01u) != 0u) {
        vec2 edgeDist = min(abs(quadPos + 1.0), abs(1.0 - quadPos));
        float borderWidth = 0.02;
        float minEdgeDist = min(edgeDist.x, edgeDist.y);

        if (minEdgeDist < borderWidth) {
            fragColor = vColor;
        } else {
            fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        }
    } else {
        fragColor = vColor;
    }
}
