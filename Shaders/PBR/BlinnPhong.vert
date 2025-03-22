#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;
layout (location = 3) in vec3 aTangent; // Tangent vector used for normal mapping, defines surface direction

layout (std140, binding = 0) uniform CameraBlock
{
  mat4 u_view;
  mat4 u_projection;
};
uniform mat4 u_model;


out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos; // Fragment position in world space

void main()
{
  mat3 normalMatrix = mat3(transpose(inverse(u_model)));  // Removes non-uniform scaling effects
  Normal = normalize(normalMatrix * aNormal);             // Transform normal to world space
  FragPos = vec3(u_model * vec4(aPos, 1.0));              // Compute world-space fragment position
  TexCoord = aUv;

  gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0);
}