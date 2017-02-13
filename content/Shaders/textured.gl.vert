#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 uv;

uniform mat3 projection = mat3(1); 

out vec2 UV;

void main() 
{
    gl_Position.xyz = projection * vPos;
	gl_Position.w = 1.0;

	UV = uv;
}
