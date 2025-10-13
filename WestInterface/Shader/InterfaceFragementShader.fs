#version 460 core

#extension GL_EXT_gpu_shader4 : enable

in flat vec4 vColor;
in flat uint vFlags;
in vec2 TexCoord;

out vec4 fragColor;

void main() {
    if ((vFlags & 0x01u) != 0u) {
        float borderWidth = 0.01;
        float maxX = 1.0 - borderWidth;
        float minX = borderWidth;
        float maxY = 1.0 - borderWidth;
        float minY = borderWidth;
        if (TexCoord.x < maxX && TexCoord.x > minX && TexCoord.y < maxY && TexCoord.y > minY) {
            discard; 
        } else {
            fragColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
    } else {     
        fragColor = vColor;
    }
}
