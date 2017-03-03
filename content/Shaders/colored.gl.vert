#version 330 core

layout(location = 0) in vec3 vPos;

uniform mat3 projection = mat3(1);

void main() 
{
    gl_Position.xyz = projection * vPos;
	gl_Position.w = 1.0;
}
