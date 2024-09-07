#version 460

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUv;

out vec2 TexCoords;

uniform mat4 u_projection;

void main()
{
	TexCoords = aUv;
	gl_Position = u_projection * vec4(aPos, 0.0, 1.0);
}