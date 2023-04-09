#version 450

layout (location = 0) in VS_OUT {
	vec2 texCoord;
} fs_in;
layout (location = 0) out vec4 finalOutputColor;

void main()
{
	finalOutputColor = vec4(fs_in.texCoord, 1.0, 1.0);
}	