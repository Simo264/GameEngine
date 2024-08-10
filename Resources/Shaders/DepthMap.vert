#version 460

layout (location = 0) in vec3 aPos;

uniform mat4 u_lightView;
uniform mat4 u_lightProjection;
uniform mat4 u_model;

void main()
{
	gl_Position = u_lightProjection * u_lightView * u_model * vec4(aPos, 1.0);
}  