#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 aInstanceModel;

uniform mat4 UView;
uniform mat4 UProjection;

void main()
{
  gl_Position = UProjection * UView * aInstanceModel * vec4(aPos, 1.0f);
}