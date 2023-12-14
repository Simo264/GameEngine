/* ---------- Instancing ---------- */
/* -------------------------------- */

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 Color;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  Color       = vec3(0.5, 0.0, 0.5);
  TexCoord    = aTexCoords;
  gl_Position = projection * view * model * vec4(aPos, 1.0f);
}