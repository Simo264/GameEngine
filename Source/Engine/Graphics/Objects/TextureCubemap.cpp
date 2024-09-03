#include "TextureCubemap.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

TextureCubemap::TextureCubemap()
	: id{ 0 },
    target{ GL_TEXTURE_CUBE_MAP }
{}

void TextureCubemap::Create()
{
	glCreateTextures(target, 1, &id);
}

void TextureCubemap::CreateStorage(i32 internalFormat, i32 width, i32 height) const
{
  glTextureStorage2D(id, 1, internalFormat, width, height);
}

void TextureCubemap::SubImage3D(
  i32 level,
  i32 xoffset,
  i32 yoffset,
  i32 zoffset,
  i32 width,
  i32 height,
  i32 depth,
  i32 format,
  i32 type,
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
void TextureCubemap::BindTextureUnit(i32 unit) const
{
  glBindTextureUnit(unit, id);
}

void TextureCubemap::SetParameteri(i32 name, i32 value) const
{
  glTextureParameteri(id, name, value);
}
void TextureCubemap::SetParameteriv(i32 name, i32* values) const
{
  glTextureParameteriv(id, name, values);
}
void TextureCubemap::SetParameterf(i32 name, f32 value) const
{
  glTextureParameterf(id, name, value);
}
void TextureCubemap::SetParameterfv(i32 name, f32* values) const
{
  glTextureParameterfv(id, name, values);
}

void TextureCubemap::LoadImages(const Array<Texture2D*, 6>& images) const
{
  i32 nrChannels = images.at(0)->nChannels;
  i32 width = images.at(0)->width;
  i32 height = images.at(0)->height;
  i32 bufsize = width * height * nrChannels;
  i32 format = images.at(0)->format;
  Vector<Byte> pixels(bufsize);

  for (i32 i = 0; i < 6; i++)
  {
    auto* texture = images.at(i);

    pixels.clear();
    texture->GetTextureImage(0, GL_UNSIGNED_BYTE, bufsize, reinterpret_cast<void*>(pixels.data()));

    SubImage3D(0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(pixels.data()));
  }
}
