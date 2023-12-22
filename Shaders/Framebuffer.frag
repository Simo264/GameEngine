#version 330 core

in vec2 TextCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
	vec3 col	= texture(screenTexture, TextCoords).rgb;
  FragColor = vec4(col, 1.0);
}