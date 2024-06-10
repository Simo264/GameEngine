#include "TextureCubemap.hpp"

#include "Core/OpenGL.hpp"

#include <stb_image.h>

TextureCubemap::TextureCubemap()
	: id{ static_cast<uint32_t>(-1) },
    target{ GL_TEXTURE_CUBE_MAP }
{}

void TextureCubemap::Create()
{
	glCreateTextures(target, 1, &id);
}
void TextureCubemap::Delete()
{
  glDeleteTextures(1, &id);
  id = static_cast<uint32_t>(-1);
}
void TextureCubemap::Bind() const
{
  glBindTexture(target, id);
}
void TextureCubemap::Unbind() const
{
  glBindTexture(target, 0);
}
void TextureCubemap::BindTextureUnit(int unit) const
{
  glBindTextureUnit(unit, id);
}

void TextureCubemap::SetParameteri(int name, int value) const
{
  glTextureParameteri(id, name, value);
}
void TextureCubemap::SetParameteriv(int name, int* values) const
{
  glTextureParameteriv(id, name, values);
}
void TextureCubemap::SetParameterf(int name, float value) const
{
  glTextureParameterf(id, name, value);
}
void TextureCubemap::SetParameterfv(int name, float* values) const
{
  glTextureParameterfv(id, name, values);
}

void TextureCubemap::LoadImages(const array<fspath, 6>& images)
{
  Bind();
  for (int i = 0; i < 6; i++)
  {
    int width, height, nrChannels;
    auto data = stbi_load(images[i].string().c_str(), &width, &height, &nrChannels, 0);
    
    int internalformat{ GL_R8 }, format{ GL_RED };
    switch (nrChannels) 
    {
    case 1:
      internalformat = GL_R8;
      format = GL_RED;
      break;
    case 2:
      internalformat = GL_RG8;
      format = GL_RG;
      break;
    case 3:
      internalformat = GL_RGB8;
      format = GL_RGB;
      break;
    case 4:
      internalformat = GL_RGBA8;
      format = GL_RGBA;
      break;
    }

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
  }
  Unbind();
}
