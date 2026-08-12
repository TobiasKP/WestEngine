#version 410 core

in vec3 direction;
out vec4 fragColor;

uniform samplerCube skybox;

void main()
{
    fragColor = texture(skybox, direction);
}
