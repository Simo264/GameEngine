#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

layout (std140, binding = 0) uniform cameraBlock
{
  mat4 u_view;
  mat4 u_projection;
};
uniform mat4 u_model;
uniform vec3 u_viewPos;

vec3 lightPos = vec3(-1.0f, 0.0f, 2.0f);

void main()
{
  vec3 T   = normalize(mat3(u_model) * aTangent);
  vec3 B   = normalize(mat3(u_model) * aBitangent);
  vec3 N   = normalize(mat3(u_model) * aNormal);
  mat3 TBN = transpose(mat3(T, B, N));
  
  TangentLightPos = TBN * lightPos;
  TangentViewPos  = TBN * u_viewPos;
  TangentFragPos  = TBN * FragPos;

  FragPos = vec3(u_model * vec4(aPos, 1.0)); 
  TexCoords = aTexCoords;
  gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0);
}