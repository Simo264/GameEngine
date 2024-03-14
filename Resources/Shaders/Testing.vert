#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 UModel;
uniform mat4 UView;
uniform mat4 UProjection;

void main()
{
  TexCoords = aTexCoords;    
  gl_Position = UProjection * UView * UModel * vec4(aPos*1.05f, 1.0f);
}