#version 330 core

out vec4 FragColor;

in vec2 TC;
in int BID;
in float FL;

uniform sampler2D test;
uniform vec3 color[70];

void main()
{
    FragColor = texture(test, TC) * (vec4(color[BID], 1.0) * FL);
    //FragColor = texture(test, TC);
}