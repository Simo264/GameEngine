#include "TextureCubemap.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

#include <stb_image.h>

TextureCubemap::TextureCubemap()
	: id{ static_cast<uint32_t>(-1) },
    target{ GL_TEXTURE_CUBE_MAP },
    size{ 0 }
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
  if (size == 0)
  {
    CONSOLE_WARN("Invalid texture size");
    return;
  }

  Bind();
  glTextureStorage2D(id, 1, GL_RGB8, size, size);
  for (int i = 0; i < 6; i++)
  {
    int width{ 0 }, height{ 0 }, nrChannels{ 0 };
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
    
    glTextureSubImage3D(id, 0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
  }
  Unbind();
}
