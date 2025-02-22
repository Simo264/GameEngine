#include "TextureCubemap.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Subsystems/TexturesManager.hpp"

static bool IsValidTexture2DFormat(Texture2DFormat format2d)
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
static Texture3DFormat ConvertToTexture3DFormat(Texture2DFormat format2d)
{
  if (!IsValidTexture2DFormat(format2d))
    throw std::invalid_argument("error on ConvertToTexture3DFormat: unsupported Texture2DFormat for Texture3DFormat.");

  return static_cast<Texture3DFormat>(static_cast<u32>(format2d));
}



void TextureCubemap::Create()
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &id);
}

void TextureCubemap::CreateStorage(Texture2DInternalFormat internalFormat, 
                                   i32 width, 
                                   i32 height) const
{
  glTextureStorage2D(id, 
    1, 
    static_cast<u32>(internalFormat),
    width, 
    height);
}

void TextureCubemap::Delete()
{
  glDeleteTextures(1, &id);
  id = 0;
}

bool TextureCubemap::IsValid() const
{
  return (id != 0) && (glIsTexture(id) == GL_TRUE);
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

void TextureCubemap::SetParameteri(TextureParameteriName name, 
                                   TextureParameteriParam value) const
{
  glTextureParameteri(
    id,
    static_cast<u32>(name),
    static_cast<i32>(value));
}

void TextureCubemap::LoadImages(const Array<Texture2D, 6>& images) const
{
  i32 width = images.at(0).GetWidth();
  i32 height = images.at(0).GetHeight();
  Texture2DInternalFormat internalFormat = images.at(0).GetInternalFormat();
  Texture2DFormat format2d = images.at(0).GetFormat(internalFormat);
  Texture3DFormat format3d = ConvertToTexture3DFormat(format2d);

  i32 nrChannels = images.at(0).GetNumChannels(internalFormat);
  i32 bufsize = width * height * nrChannels;
  Vector<u8> pixels(bufsize);
  for (i32 i = 0; i < 6; i++)
  {
    Texture2D texture = images.at(i);
    pixels.clear();
    
    texture.GetTextureImage(0,
      format2d,
      Texture2DGetImageType::UNSIGNED_BYTE, 
      bufsize, 
      reinterpret_cast<void*>(pixels.data()));
    
    SubImage3D(0,
      0,
      0,
      i,
      width,
      height,
      1,
      format3d,
      Texture3DType::UNSIGNED_BYTE,
      reinterpret_cast<void*>(pixels.data()));
  }
}
