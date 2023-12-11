/* ---------- Instancing ---------- */
/* -------------------------------- */

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 aModel;

/*
  layout (location = 1) in mat4 aModel; 
  mat4 = vec4 x 4 = (layout 1) in vec4 
                    (layout 2) in vec4 
                    (layout 3) in vec4 
                    (layout 4) in vec4 
*/


uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * aModel * vec4(aPos, 1.0f);
}