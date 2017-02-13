#version 330 core
layout (location = 0) in vec3 pos;

layout (location = 1) in vec2 UV;
out vec2 TexCoords;

uniform mat3 projection;

void main()
{
    gl_Position.xyz = projection * pos;
    gl_Position.w = 1;

    TexCoords = UV;
}  