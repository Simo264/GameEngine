#version 330 core
in vec3 Color;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
  FragColor = vec4(Color, 1.0);
  //FragColor = texture(ourTexture, TexCoord);
}