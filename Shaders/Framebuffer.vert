#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTextCoords;

out vec2 TextCoords;

void main()
{
  TextCoords  = aTextCoords;
  gl_Position = vec4(aPos, 0.0f, 1.0f);
}