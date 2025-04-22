#version 460 core

out vec4 OutColor;

uniform float u_zNear; 
uniform float u_zFar; 
const float g_gamma = 2.2f;

float LinearizeDepth(float depth, float near, float far)
{
  float z = depth * 2.0f - 1.0f; // Back to NDC 
  return (2.0f * near * far) / (far + near - z * (far - near));	
}

void main()
{             
  float linearDepth = LinearizeDepth(gl_FragCoord.z, u_zNear, u_zFar) / u_zFar;
  float color = 1.0 - linearDepth;
  color = pow(color, g_gamma);
  OutColor = vec4(vec3(color), 1.0);
}
