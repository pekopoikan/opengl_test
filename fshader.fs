#version 330 core

out vec4 FragColor;

in vec2 TC;

uniform sampler2D test;

void main()
{
    FragColor = texture(test, TC); //* vec4(3.0, 1.0, 7.0, 1.0);
}