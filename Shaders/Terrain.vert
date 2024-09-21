#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUv;

out vec2 TexCoord;

void main()
{
  /* Convert XYZ vertex to XYZW homogeneous coordinate */ 
  gl_Position = vec4(aPos, 1.0);
  /* Pass uv coordinate though */ 
  TexCoord = aUv;
}