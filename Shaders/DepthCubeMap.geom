#version 460

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 u_lightProjection;
uniform mat4 u_lightViews[6];

out vec4 FragPos; /* FragPos from GS (output per emitvertex) */ 

void main()
{
  for(int face = 0; face < 6; face++)
  {
    /* built-in variable that specifies to which face we render */
    gl_Layer = face; 
    
    /* for each triangle vertex */ 
    for(int i = 0; i < 3; i++) 
    {
      FragPos = gl_in[i].gl_Position;
      gl_Position = u_lightProjection * u_lightViews[face] * FragPos;
      EmitVertex();
    }    
    EndPrimitive();
  }
} 