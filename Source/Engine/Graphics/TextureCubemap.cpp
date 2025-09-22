#include "TextureCubemap.hpp"
#include <glad/gl.h>

void TextureCubemap::Create()
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &id);
}

void TextureCubemap::CreateStorage(Texture2DInternalFormat internalFormat, i32 width, i32 height) const
{
  glTextureStorage2D(id, 1, static_cast<u32>(internalFormat), width, height);
}

void TextureCubemap::Release()
{
  glDeleteTextures(1, &id);
  id = INVALID_TEXTURE_ID;
}

void TextureCubemap::SubImage3D(i32 level,
                                i32 xoffset,
                                i32 yoffset,
                                i32 zoffset,
                                i32 width,
                                i32 height,
                                i32 depth,
                                Texture3DFormat format,
                                Texture3DType type,
                                const void* pixels) const
{
  glTextureSubImage3D(id,
                      level,
                      xoffset,
                      yoffset,
                      zoffset,
                      width,
                      height,
                      depth,
                      static_cast<u32>(format),
                      static_cast<u32>(type),
                      pixels);
}

void TextureCubemap::BindTextureUnit(i32 unit) const
{
  glBindTextureUnit(unit, id);
}

void TextureCubemap::SetParameteri(TextureParameteriName name, TextureParameteriParam value) const
{
  glTextureParameteri(id, static_cast<u32>(name), static_cast<i32>(value));
}

void TextureCubemap::LoadImages(const Array<Texture2D, 6>& images) const
{
  auto width = images.at(0).GetWidth();
  auto height = images.at(0).GetHeight();
  auto format2d = images.at(0).GetFormat();
  auto format3d = __ConvertToTexture3DFormat(format2d);
  auto nrChannels = images.at(0).GetNumChannels();
  auto buffSize = width * height * nrChannels;
  
  auto pixels = std::make_unique<byte[]>(buffSize);
  for (auto i = 0u; i < 6; i++)
  {
    std::fill_n(pixels.get(), buffSize, static_cast<byte>(0));

    auto& texture = images.at(i);
    texture.GetTextureImage(0,
                            Texture2DGetImageType::UNSIGNED_BYTE, 
                            buffSize,
                            reinterpret_cast<void*>(pixels.get()));
    
    SubImage3D(0,
               0,
               0,
               i,
               width,
               height,
               1,
               format3d,
               Texture3DType::UNSIGNED_BYTE,
               reinterpret_cast<void*>(pixels.get()));
  }
}



bool TextureCubemap::__IsValidTexture2DFormat(Texture2DFormat format2d) const
{
  switch (format2d)
  {
    case Texture2DFormat::RED:
    case Texture2DFormat::RG:
    case Texture2DFormat::RGB:
    case Texture2DFormat::RGBA:
    case Texture2DFormat::DEPTH_COMPONENT:
    case Texture2DFormat::RED_INTEGER:
    case Texture2DFormat::RG_INTEGER:
    case Texture2DFormat::RGB_INTEGER:
    case Texture2DFormat::RGBA_INTEGER:
    case Texture2DFormat::DEPTH_STENCIL:
      return true;
    default:
      return false;
  }
}
Texture3DFormat TextureCubemap::__ConvertToTexture3DFormat(Texture2DFormat format2d) const
{
  if (!__IsValidTexture2DFormat(format2d))
    throw std::invalid_argument("error on ConvertToTexture3DFormat: unsupported Texture2DFormat for Texture3DFormat.");

  return static_cast<Texture3DFormat>(static_cast<u32>(format2d));
}

