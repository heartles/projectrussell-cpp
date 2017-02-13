#version 330 core

in vec2 UV; 

uniform sampler2D tex;
uniform vec4 color = vec4(1);

out vec4 outColor;

void main() {
	outColor = texture(tex, UV) * color;
}