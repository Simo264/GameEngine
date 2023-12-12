#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
  //FragColor = vec4(0.25, 0.50, 0.75, 1.0);
  FragColor = texture(ourTexture, TexCoord);
}