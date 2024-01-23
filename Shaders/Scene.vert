#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normals;
out vec2 TexCoords;
out vec3 FragPos;

uniform mat4 UModel;
uniform mat4 UView;
uniform mat4 UProjection;

void main()
{
  FragPos     = vec3(UModel * vec4(aPos, 1.0));
  Normals     = aNormals;
  TexCoords   = aTexCoords;
  gl_Position = UProjection * UView * UModel * vec4(aPos, 1.0f);
}