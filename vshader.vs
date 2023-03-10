#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in int id;
layout (location = 4) in float weight;

out vec2 TC;
out float FL;
out int BID;

uniform mat4 projection;

uniform mat4 view;

uniform mat4 model;

uniform mat4 transformation;

void main()
{
    gl_Position = projection * view * model * transformation * vec4(aPos, 1.0);
    TC = aTex;
    BID = id;
    FL = weight;
}