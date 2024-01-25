#version 460

layout (location = 0) in vec3 aPos;

uniform mat4 ULightSpaceMatrix;
uniform mat4 UModel;

void main()
{
	gl_Position = ULightSpaceMatrix * UModel * vec4(aPos, 1.0);
}  