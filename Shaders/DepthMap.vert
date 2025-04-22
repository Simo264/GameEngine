#version 460 core

layout (location = 0) in vec3 aPos;

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
  gl_Position = u_cameraProjection * u_cameraView * u_model * vec4(aPos, 1.0);
}