#version 460

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;

void main()
{             
  float depthValue = texture(depthMap, TexCoords).r;
  FragColor = vec4(vec3(depthValue), 1.0);
}