#version 460

layout(location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform cameraBlock
{
  mat4 u_view;
  mat4 u_projection;
};

out vec3 NearPoint;
out vec3 FarPoint;
out mat4 View;
out mat4 Projection;

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) 
{
  mat4 viewInv = inverse(view);
  mat4 projInv = inverse(projection);
  vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
  return unprojectedPoint.xyz / unprojectedPoint.w;
}


void main()
{
  vec3 p = aPos;
  NearPoint = UnprojectPoint(p.x, p.y, 0.0, u_view, u_projection).xyz; // unprojecting on the near plane
  FarPoint = UnprojectPoint(p.x, p.y, 1.0, u_view, u_projection).xyz; // unprojecting on the far plane
  
  View = u_view;
  Projection = u_projection;
  
  gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}