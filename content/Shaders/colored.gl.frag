#version 330 core

uniform vec4 color = vec4(1);
out vec4 outColor;

void main() {
	outColor = color;
}