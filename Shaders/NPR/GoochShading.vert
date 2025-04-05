#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;
layout (location = 3) in vec3 aTangent; // Tangent vector used for normal mapping, defines surface direction

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos; // Fragment position in world space
out vec3 ViewPos;

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
  mat3 normalMatrix = mat3(transpose(inverse(u_model))); // Removes non-uniform scaling effects
  vec3 N = normalize(normalMatrix * aNormal);            // Transform normal to world space

  vec4 p = vec4(aPos, 1.0);

  FragPos = vec3(u_model * p); // Compute world-space fragment position
  TexCoord = aUv;
  ViewPos = u_cameraPosition;
  Normal = N;

  gl_Position = u_cameraProjection * u_cameraView * u_model * p;
}