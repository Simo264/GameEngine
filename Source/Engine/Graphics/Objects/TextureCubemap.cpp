#include "TextureCubemap.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"
#include "Engine/Subsystems/TexturesManager.hpp"

void TextureCubemap::Create()
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &id);
}

void TextureCubemap::CreateStorage(Texture2DInternalFormat internalFormat, i32 width, i32 height) const
{
  glTextureStorage2D(
    id, 
    1, 
    static_cast<u32>(internalFormat),
    width, 
    height);
}

bool TextureCubemap::IsValid() const
{
  return (id != 0) && (glIsTexture(id) == GL_TRUE);
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

void TextureCubemap::BindTextureUnit(i32 unit) const
{
  glBindTextureUnit(unit, id);
}

void TextureCubemap::SetParameteri(TextureParameteriName name, TextureParameteriParam value) const
{
  glTextureParameteri(
    id,
    static_cast<u32>(name),
    static_cast<i32>(value));
}

void TextureCubemap::LoadImages(const Array<const Texture2D*, 6>& images) const
{
  i32 nrChannels = images.at(0)->nChannels;
  i32 width = images.at(0)->width;
  i32 height = images.at(0)->height;
  i32 bufsize = width * height * nrChannels;
  i32 format = images.at(0)->format;
  Vector<u8> pixels(bufsize);

  for (i32 i = 0; i < 6; i++)
  {
    const auto* texture = images.at(i);

    pixels.clear();
    texture->GetTextureImage(0, GL_UNSIGNED_BYTE, bufsize, reinterpret_cast<void*>(pixels.data()));

    SubImage3D(0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(pixels.data()));
  }
}
