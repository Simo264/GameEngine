#include "TextureCubemap.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

TextureCubemap::TextureCubemap()
	: id{ 0 },
    target{ GL_TEXTURE_CUBE_MAP }
{}

void TextureCubemap::Create()
{
	glCreateTextures(target, 1, &id);
}

void TextureCubemap::CreateStorage(int internalFormat, int width, int height) const
{
  glTextureStorage2D(id, 1, internalFormat, width, height);
}

void TextureCubemap::SubImage3D(
  int level,
  int xoffset,
  int yoffset,
  int zoffset,
  int width,
  int height,
  int depth,
  int format,
  int type,
  const void* pixels) const
{
  glTextureSubImage3D(id, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}

void TextureCubemap::Delete()
{
  glDeleteTextures(1, &id);
  id = 0;
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

void TextureCubemap::LoadImages(const array<Texture2D*, 6>& images) const
{
  int nrChannels = images.at(0)->nrChannels;
  int width = images.at(0)->width;
  int height = images.at(0)->height;
  int bufsize = width * height * nrChannels;
  int format = images.at(0)->format;
  vector<byte> pixels(bufsize);

  for (int i = 0; i < 6; i++)
  {
    auto* texture = images.at(i);

    pixels.clear();
    texture->GetTextureImage(0, GL_UNSIGNED_BYTE, bufsize, reinterpret_cast<void*>(pixels.data()));

    SubImage3D(0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(pixels.data()));
  }
}
