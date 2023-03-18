#version 330 core

out vec4 FragColor;

in vec2 TC;
in int ID;

uniform sampler2D test;
uniform vec3 color[70];

void main()
{
    FragColor = texture(test, TC) * color[ID];
}