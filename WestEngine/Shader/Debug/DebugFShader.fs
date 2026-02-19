#version 460 core

out vec4 fragColor;

uniform vec3 ddColor;

void main() {
    fragColor = vec4(ddColor, 1.0);
}
