#version 450 core

precision highp float;
precision highp int;

layout ( location = 0 ) in VSOUT {
	vec2 texCoord;
} fs_in;
layout (location = 0) out vec4 outFragCoord;

// TODO(snowapril) : modify to input attachment
layout(set = 1, binding = 0) uniform sampler2D g_sceneColor;

vec4 SRGBtoLinear(vec4 srgbIn, float gamma)
{
	return vec4(pow(srgbIn.xyz, vec3(gamma)), srgbIn.w);
}

vec3 Uncharted2Tonemap(vec3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}

vec4 tonemap(vec4 color, float gamma, float exposure)
{
	vec3 outcol = Uncharted2Tonemap(color.rgb * exposure);
	outcol = outcol * (1.0f / Uncharted2Tonemap(vec3(11.2f)));
	return vec4(pow(outcol, vec3(1.0f / gamma)), color.a);
}


void main()
{
	vec4 sceneColor = texture(g_sceneColor, fs_in.texCoord);
	outFragCoord = vec4(Uncharted2Tonemap(sceneColor.xyz), 1.0f);
}