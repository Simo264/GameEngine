#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_lightSpaceMatrix;

void main()
{
  FragPos           = vec3(u_model * vec4(aPos, 1.0));
  Normal            = aNormal;
  TexCoords         = aTexCoords;
  FragPosLightSpace = u_lightSpaceMatrix * vec4(FragPos, 1.0);
  gl_Position       = u_projection * u_view * u_model * vec4(aPos, 1.0);
}