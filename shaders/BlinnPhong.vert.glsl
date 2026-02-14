#version 460

layout (location = 0) in vec3 in_VertexPos;
layout (location = 1) in vec3 in_VertexNormal;
layout (location = 2) in vec2 in_VertexTc;
layout (location = 3) in vec3 in_VertexTangent; // vertex tangent vector (used for normal mapping)

layout (std140, binding = 0) uniform CameraBlock
{
  mat4 u_CameraView;
  mat4 u_CameraProjection;
  vec4 u_CameraPosition; // vec4 needed for alignment std140
};

layout (location = 0) uniform mat4 u_Transform;

out vec2 vs_out_TextureCoord;
out vec3 vs_out_Normal;
out vec3 vs_out_FragPos;
out vec3 vs_out_CameraPos;
out mat3 vs_out_TBN;

void main()
{
  // Removes non-uniform scaling effects
  mat3 normalMatrix = mat3(transpose(inverse(u_Transform)));
  // Transform normal to world space
  vec3 n = normalMatrix * in_VertexNormal;

  // Compute world-space fragment position
  vs_out_FragPos = vec3(u_Transform * vec4(in_VertexPos, 1.0));
  vs_out_TextureCoord = in_VertexTc;
  vs_out_CameraPos = u_CameraPosition.xyz;
  vs_out_Normal = n;

  // Transform tangent to world space
  vec3 t = normalize(mat3(u_Transform) * in_VertexTangent);
  // Re-orthogonalize tangent to ensure perpendicularity to normal
  t = normalize(t - dot(t, n) * n);
  vec3 b = cross(n, t);
  vs_out_TBN = mat3(t, b, n);

  gl_Position = u_CameraProjection * u_CameraView * u_Transform * vec4(in_VertexPos, 1.0);
}