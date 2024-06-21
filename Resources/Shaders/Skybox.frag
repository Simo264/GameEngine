#version 460

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube u_skyboxTexture;

void main()
{    
  FragColor = texture(u_skyboxTexture, TexCoords);
}