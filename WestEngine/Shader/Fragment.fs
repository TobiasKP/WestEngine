#version 460 core

in vec2 fragTextureCoord;

out vec4 fragColour;

//uniform sampler2D textureSampler;
uniform vec3 dColor;

void main() {
    fragColour = vec4(dColor, 1.0); //texture(textureSampler, fragTextureCoord);
}
