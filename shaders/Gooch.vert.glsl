#version 460

layout (location = 0) in vec3 in_VertexPos;
layout (location = 1) in vec3 in_VertexNormal;

layout (std140, binding = 0) uniform CameraBlock
{
  mat4 u_CameraView;
  mat4 u_CameraProjection;
  vec4 u_CameraPosition; // vec4 needed for std140 alignment
};

layout (location = 0) uniform mat4 u_Transform;

out vec3 vs_out_Normal;
out vec3 vs_out_FragPos;
out vec3 vs_out_CameraPos;

void main()
{
  // Removes non-uniform scaling effects
  mat3 normalMatrix = mat3(transpose(inverse(u_Transform)));
  // Transform normal to world space
  vec3 n = normalMatrix * in_VertexNormal;

  // Compute world-space fragment position
  vs_out_FragPos = vec3(u_Transform * vec4(in_VertexPos, 1.0));
  vs_out_CameraPos = u_CameraPosition.xyz;
  vs_out_Normal = n;                             

  gl_Position = u_CameraProjection * u_CameraView * u_Transform * vec4(in_VertexPos, 1.0);
}