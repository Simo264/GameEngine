#version 460

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D u_text;
uniform vec3 u_textColor;

void main()
{    
  vec4 sampled = vec4(vec3(1), texture(u_text, TexCoords).r);
  FragColor = vec4(u_textColor, 1.0) * sampled;
} 