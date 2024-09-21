#version 460

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUv;

out vec2 TextCoords;

void main()
{
  TextCoords  = aUv;
  gl_Position = vec4(aPos, 0.0f, 1.0f);
}