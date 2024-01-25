#version 460
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D UShadowMap;
uniform float UNearPlane;
uniform float UFarPlane;

void main()
{             
  float depthValue = texture(UShadowMap, TexCoords).r;
  FragColor = vec4(vec3(depthValue), 1.0); /* Orthographic */
}