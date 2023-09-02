#version 450 core

precision mediump float;

layout(location = 0) in vec2 aTexCoord;
layout(set = 2, binding = 0) uniform sampler2D uTex;
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = textureProj(uTex, vec4(aTexCoord, 0.0, 1.0));
}