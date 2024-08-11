#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;
out vec3 ViewPos;
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

/* Directional light matrices */
uniform mat4 u_lightView;
uniform mat4 u_lightProjection;

void main()
{
  mat3 normalMatrix = mat3(transpose(inverse(u_model)));
  vec3 N = normalize(normalMatrix * aNormal);
  vec3 T = normalize(mat3(u_model) * aTangent);
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);
  TBN = transpose(mat3(T,B,N));

  FragPos = vec3(u_model * vec4(aPos, 1.0));
  TexCoord = aTexCoord;
  ViewPos = u_viewPos;
  Normal = N;
	FragPosLightSpace = u_lightProjection * u_lightView * vec4(FragPos, 1.0);

  TangentViewPos = TBN * ViewPos;
  TangentFragPos = TBN * FragPos;

	gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0f);
}