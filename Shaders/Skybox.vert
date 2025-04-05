#version 460

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140, binding = 0) uniform CameraBlock
{
  mat4 u_cameraView;
  mat4 u_cameraProjection;
  vec3 u_cameraPosition;
  float __padding;
};

void main()
{
  TexCoords = aPos;
  mat4 view = mat4(mat3(u_cameraView));
  vec4 pos = u_cameraProjection * view * vec4(aPos, 1.0);
  gl_Position = pos.xyww;
}  