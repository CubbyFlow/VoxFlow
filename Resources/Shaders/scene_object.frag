#version 450 core

precision highp float;
precision highp int;

layout ( location = 0 ) in VSOUT {
	vec3 color;
} fs_in;
layout (location = 0) out vec4 outFragCoord;

void main()
{
	vec3 color = fs_in.color + vec3(0.5);
	outFragCoord = vec4(color, 1.0f);
}