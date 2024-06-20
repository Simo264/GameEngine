#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPos;
out mat3 TBN;

layout (std140, binding = 0) uniform cameraBlock
{
  mat4 u_view;
  mat4 u_projection;
};
uniform mat4 u_model;

void main()
{
  vec3 T = normalize(mat3(u_model) * aTangent);
  vec3 B = normalize(mat3(u_model) * aBitangent);
  vec3 N = normalize(mat3(u_model) * aNormal);
  TBN    = transpose(mat3(T, B, N));

  FragPos     = vec3(u_model * vec4(aPos, 1.0));
  Normal      = mat3(transpose(inverse(u_model))) * aNormal;  
  TexCoords   = aTexCoords;
  gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0f);
}