#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;
layout (location = 3) in vec3 aTangent; // vertex tangent vector (used for normal mapping)

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 CameraPos;
out mat3 TBN;

layout (std140, binding = 0) uniform CameraBlock
{
  mat4 u_cameraView;
  mat4 u_cameraProjection;
  vec3 u_cameraPosition;
  float __padding;
};
uniform mat4 u_model;

void main()
{
  mat3 normalMatrix = mat3(transpose(inverse(u_model)));  // Removes non-uniform scaling effects
  vec3 N = normalMatrix * aNormal;                        // Transform normal to world space

  vec4 p = vec4(aPos, 1.0);
  FragPos = vec3(u_model * p);
  CameraPos = u_cameraPosition;
  TexCoord = aUv;
  Normal = N;

  vec3 T = normalize(mat3(u_model) * aTangent); // Transform tangent to world space
  T = normalize(T - dot(T, N) * N);             // Re-orthogonalize tangent to ensure perpendicularity to normal
  vec3 B = cross(N, T);                         // Compute bitangent using cross product
  TBN = mat3(T,B,N);                            // Construct the TBN matrix

  gl_Position = u_cameraProjection * u_cameraView * u_model * p;
}