#version 460 core

out vec4 FragColor;

in float Height;

void main()
{
  /* shift and scale the height into a grayscale value */
  float h = (Height + 16)/64.0f;
  FragColor = vec4(h, h, h, 1.0);
}