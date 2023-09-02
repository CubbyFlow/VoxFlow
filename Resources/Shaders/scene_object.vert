#version 450 core

precision highp float;
precision highp int;

layout(location = 0) in vec3 aPosition;
layout ( location = 0 ) out VSOUT{
	vec3 color;
} vs_out;

void main() {
	vs_out.color = aPosition;
	gl_Position = vec4(aPosition, 1);
}