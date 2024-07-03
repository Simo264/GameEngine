#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec4 FragPosLightSpace;
out mat3 TBN;

layout (std140, binding = 0) uniform cameraBlock
{
  mat4 u_view;
  mat4 u_projection;
};
uniform mat4 u_model;
uniform mat4 u_lightSpaceMatrix;

void main()
{
  mat3 normalMatrix = transpose(inverse(mat3(u_model)));
  vec3 N = normalize(normalMatrix * aNormal);
  vec3 T = normalize(normalMatrix * aTangent);
  vec3 B = normalize(cross(N, T));
  TBN    = mat3(T, B, N);

	FragPos           = vec3(u_model * vec4(aPos, 1.0));
	TexCoords         = aTexCoords;
	FragPosLightSpace = u_lightSpaceMatrix * vec4(FragPos, 1.0);
	gl_Position       = u_projection * u_view * u_model * vec4(aPos, 1.0);
}