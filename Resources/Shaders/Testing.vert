#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in mat4 instanceModel;

out vec3 fColor;

layout (std140, binding = 0) uniform cameraBlock
{
  mat4 u_view;
  mat4 u_projection;
};

void main()
{
  gl_Position = u_projection * u_view * instanceModel * vec4(aPos, 1.0f);
  fColor = aColor;
}