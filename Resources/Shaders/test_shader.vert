#version 450 core

precision mediump float;

layout(set = 1, binding = 5) uniform sampler2D textures[5];
layout(location = 0) in vec2 aTexCoord;
layout(location = 0) out vec4 vTex;

void main()
{
   vTex = vec4(1.0);
   gl_Position = texture(textures[2], aTexCoord);
}