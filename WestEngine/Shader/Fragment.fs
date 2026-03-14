#version 410 core

in vec2 fragTextureCoord;

out vec4 fragColour;

//uniform sampler2D textureSampler;
uniform vec3 dColor;
uniform vec3 keColor;

void main() {
    fragColour = vec4(dColor + keColor, 1.0); //texture(textureSampler, fragTextureCoord);
}
