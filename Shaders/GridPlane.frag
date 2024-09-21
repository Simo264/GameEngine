#version 460

in vec3 NearPoint;
in vec3 FarPoint;
in mat4 View;
in mat4 Projection;

out vec4 FragColor;

const float zNear = 0.1f;
const float zFar  = 100.0f;

vec4 Grid(vec3 fragPos3D, float scale, bool drawAxis) 
{
  vec2 coord = fragPos3D.xz * scale;
  vec2 derivative = fwidth(coord);
  vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
  float line = min(grid.x, grid.y);
  float minimumz = min(derivative.y, 1);
  float minimumx = min(derivative.x, 1);
  vec4 color = vec4(0.2f, 0.2f, 0.2f, 1.0f - min(line, 1.0f));
  /* z-axis */
  if(fragPos3D.x > -0.1f * minimumx && fragPos3D.x < 0.1f * minimumx)
      color.z = 1.0f;
  /* x-axis */
  if(fragPos3D.z > -0.1f * minimumz && fragPos3D.z < 0.1f * minimumz)
      color.x = 1.0f;
  return color;
}

float ComputeDepth(vec3 pos) 
{
  vec4 clipSpacePos = Projection * View * vec4(pos, 1.0f);
  return (clipSpacePos.z / clipSpacePos.w) * 0.5f + 0.5f;
}

float ComputeLinearDepth(vec3 pos) 
{
  vec4 clipSpacePos = Projection * View * vec4(pos.xyz, 1.0f);
  float clipSpaceDepth = (clipSpacePos.z / clipSpacePos.w) * 2.0f - 1.0f; /* put back between -1 and 1 */ 
  float linearDepth = (2.0f * zNear * zFar) / (zFar + zNear - clipSpaceDepth * (zFar - zNear));
  return linearDepth / zFar; /* normalize */
}


void main() 
{
  float t = -NearPoint.y / (FarPoint.y - NearPoint.y);
  vec3 fragPos3D = NearPoint + t * (FarPoint - NearPoint);

  gl_FragDepth = ComputeDepth(fragPos3D);

  float linearDepth = ComputeLinearDepth(fragPos3D);
  float fading = max(0, (0.5f - linearDepth));

  FragColor = (Grid(fragPos3D, 10, true) + Grid(fragPos3D, 1, true))* float(t > 0); // adding multiple resolution for the grid
  FragColor.a *= fading;
}