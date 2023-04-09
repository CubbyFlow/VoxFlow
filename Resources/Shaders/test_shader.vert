#version 450 core

layout(rgba32f, binding = 5) uniform readonly highp imageBuffer uSampo[5];
layout(location = 0) out vec4 vTex;
void main()
{
   vTex = vec4(1.0);
   gl_Position = imageLoad(uSampo[2], 100);
}