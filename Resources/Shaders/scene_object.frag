#version 450 core

precision highp float;
precision highp int;

layout ( location = 0 ) in VSOUT {
	vec3 color;
} fs_in;
layout (location = 0) out vec4 outFragCoord;

void main()
{
	outFragCoord = vec4(fs_in.color, 1.0f);
}