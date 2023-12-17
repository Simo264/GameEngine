/* ---------- Instancing ---------- */
/* -------------------------------- */

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normals;
out vec2 TexCoords;
out vec3 FragPos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
  FragPos     = vec3(Model * vec4(aPos, 1.0));
  Normals     = aNormals;
  TexCoords   = aTexCoords;
  gl_Position = Projection * View * Model * vec4(aPos, 1.0f);
}