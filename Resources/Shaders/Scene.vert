#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out vec2 TexCoords;
out vec3 FragPos;
out mat3 TBN;
out vec3 TangentViewPos;
out vec3 TangentFragPos;


layout (std140, binding = 0) uniform cameraBlock
{
  mat4 u_view;
  mat4 u_projection;
};
uniform mat4 u_model;
uniform vec3 u_viewPos;

void main()
{
  FragPos = vec3(u_model * vec4(aPos, 1.0));
  TexCoords = aTexCoords;

  vec3 T = normalize(vec3(u_model * vec4(aTangent,0.0)));
  vec3 N = normalize(vec3(u_model * vec4(aNormal,0.0)));
  vec3 B = normalize(cross(N, T));
  TBN = transpose(mat3(T, B, N));

  TangentViewPos = TBN * u_viewPos;
  TangentFragPos = TBN * FragPos;

  gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0f);
}