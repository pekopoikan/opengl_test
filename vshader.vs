#version 330 core

#define MAX_JOINTS = 200;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

out vec2 TC;

uniform mat4 projection;

uniform mat4 view;

uniform mat4 model;

uniform mat4 transformation;

uniform mat4 bones[MAX_JOINTS];

void main()
{
    gl_Position = projection * view * model * transformation * vec4(aPos, 1.0);
    TC = aTex;
}