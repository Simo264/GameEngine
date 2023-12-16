/* ---------- Instancing ---------- */
/* -------------------------------- */

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in mat4 aModel;

out vec3 Color;
out vec2 TexCoord;

/*
  layout (location = 2) in mat4 aModel; 
  mat4 = vec4 x 4 = (layout 2) in vec4 
                    (layout 3) in vec4 
                    (layout 4) in vec4 
                    (layout 5) in vec4 
*/


uniform mat4 View;
uniform mat4 Projection;

void main()
{
  Color       = vec3(0.25, 0.50, 0.75);
  TexCoord    = aTexCoords;
  gl_Position = Projection * View * aModel * vec4(aPos, 1.0f);
}