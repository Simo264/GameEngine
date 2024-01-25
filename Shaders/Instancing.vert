#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aModel;
/*
  layout (location = i) in mat4 aModel; 
  mat4 = vec4 x 4 = (layout i) in vec4 
                    (layout i+1) in vec4 
                    (layout i+2) in vec4 
                    (layout i+3) in vec4 
*/

out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPos;

uniform mat4 UView;
uniform mat4 UProjection;

void main()
{
  FragPos     = vec3(aModel * vec4(aPos, 1.0));
  Normal      = aNormal;
  TexCoords   = aTexCoords;
  gl_Position = UProjection * UView * aModel * vec4(aPos, 1.0f);
}