#version 330 core

layout(location = 0) in vec2 vPos;

void main() 
{
    gl_Position.xy = vPos;
    gl_Position.z = 0.0;
	gl_Position.w = 1.0;
}
