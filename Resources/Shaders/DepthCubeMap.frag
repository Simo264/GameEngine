#version 460

in vec4 FragPos;

uniform vec3  u_lightPos;
uniform float u_zFar;

void main()
{
	/* get distance between fragment and light source */ 
  float lightDistance = length(FragPos.xyz - u_lightPos);
    
  /* map to [0;1] range by dividing by zFar */ 
  lightDistance = lightDistance / u_zFar;
    
  /* write this as modified depth */ 
  gl_FragDepth = lightDistance;
}