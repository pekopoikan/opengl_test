#version 330 core

in vec2 TexCoord;

out vec3 FragColor;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}