#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 aInstanceModel;

uniform mat4 View;
uniform mat4 Projection;

void main()
{
  gl_Position = Projection * View * aInstanceModel * vec4(aPos, 1.0f);
}