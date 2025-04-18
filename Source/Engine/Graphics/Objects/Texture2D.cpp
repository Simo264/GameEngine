#include "Texture2D.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

void Texture2D::Create(Texture2DTarget target)
{
  glCreateTextures(static_cast<u32>(target), 1, &id);
}
void Texture2D::Delete()
{
  glDeleteTextures(1, &id);
  id = 0;
}
bool Texture2D::IsValid() const
{
  return (id != 0) && (glIsTexture(id) == GL_TRUE);
}

void Texture2D::BindTextureUnit(i32 unit) const
{
  glBindTextureUnit(unit, id);
}
void Texture2D::GenerateMipmap() const
{
  glGenerateTextureMipmap(id);
}

void Texture2D::CreateStorage(Texture2DInternalFormat internalFormat, i32 width, i32 height) const
{
  u32 mipmapLevels = 1 + static_cast<u32>(std::floor(std::log2(std::max(width, height))));

  glTextureStorage2D(id,
                     mipmapLevels,
                     static_cast<u32>(internalFormat),
                     width,
                     height);
}
void Texture2D::CreateStorageMultisampled(Texture2DInternalFormat internalFormat,
                                          i32 samples,
                                          i32 width,
                                          i32 height) const
{
  glTextureStorage2DMultisample(id,
                                samples,
                                static_cast<u32>(internalFormat),
                                width,
                                height,
                                true);
}

void Texture2D::UpdateStorage(i32 level,
                              i32 width,
                              i32 height,
                              Texture2DSubImageType type,
                              const void *pixels,
                              i32 xoffset,
                              i32 yoffset) const
{
  glTextureSubImage2D(id,
                      level,
                      xoffset,
                      yoffset,
                      width,
                      height,
                      static_cast<u32>(GetFormat()),
                      static_cast<u32>(type),
                      pixels);
}

void Texture2D::ClearStorage(i32 level, Texture2DClearImageType type, const void *data) const
{
  glClearTexImage(id,
                  level,
                  static_cast<u32>(GetFormat()),
                  static_cast<u32>(type),
                  data);
}

void Texture2D::GetTextureImage(i32 level, Texture2DGetImageType type, i32 buffSize, void *pixels) const
{
  glGetTextureImage(id,
                    level,
                    static_cast<u32>(GetFormat()),
                    static_cast<u32>(type),
                    buffSize,
                    pixels);
}

void Texture2D::SetParameteri(TextureParameteriName name, TextureParameteriParam value) const
{
  glTextureParameteri(id, static_cast<u32>(name), static_cast<i32>(value));
}
void Texture2D::SetParameterfv(TextureParameteriName name, f32 *values) const
{
  glTextureParameterfv(id, static_cast<u32>(name), values);
}
void Texture2D::SetCompareFunc(CompareFunc func) const
{
  glTextureParameteri(id,
                      static_cast<u32>(TextureParameteriName::COMPARE_FUNC),
                      static_cast<i32>(func));
}

i32 Texture2D::GetWidth() const
{
  i32 width;
  glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_WIDTH, &width);
  return width;
}
i32 Texture2D::GetHeight() const
{
  i32 height;
  glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_HEIGHT, &height);
  return height;
}
Texture2DInternalFormat Texture2D::GetInternalFormat() const
{
  i32 internalFormat;
  glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
  return static_cast<Texture2DInternalFormat>(internalFormat);
}
Texture2DFormat Texture2D::GetFormat() const
{
  Texture2DInternalFormat internalFormat = GetInternalFormat();

  switch (internalFormat)
  {
  case Texture2DInternalFormat::R8:
  case Texture2DInternalFormat::R8_SNORM:
  case Texture2DInternalFormat::R16:
  case Texture2DInternalFormat::R16_SNORM:
  case Texture2DInternalFormat::R16F:
  case Texture2DInternalFormat::R32F:
    return Texture2DFormat::RED;

  case Texture2DInternalFormat::RG8:
  case Texture2DInternalFormat::RG8_SNORM:
  case Texture2DInternalFormat::RG16:
  case Texture2DInternalFormat::RG16_SNORM:
  case Texture2DInternalFormat::RG16F:
  case Texture2DInternalFormat::RG32F:
    return Texture2DFormat::RG;

  case Texture2DInternalFormat::RGB8:
  case Texture2DInternalFormat::RGB8_SNORM:
  case Texture2DInternalFormat::RGB16:
  case Texture2DInternalFormat::RGB16_SNORM:
  case Texture2DInternalFormat::RGB16F:
  case Texture2DInternalFormat::RGB32F:
  case Texture2DInternalFormat::R11F_G11F_B10F:
  case Texture2DInternalFormat::RGB9_E5:
  case Texture2DInternalFormat::SRGB8:
    return Texture2DFormat::RGB;

  case Texture2DInternalFormat::RGBA8:
  case Texture2DInternalFormat::RGBA8_SNORM:
  case Texture2DInternalFormat::RGBA16:
  case Texture2DInternalFormat::RGBA16_SNORM:
  case Texture2DInternalFormat::RGBA16F:
  case Texture2DInternalFormat::RGBA32F:
  case Texture2DInternalFormat::SRGB8_ALPHA8:
    return Texture2DFormat::RGBA;

  case Texture2DInternalFormat::DEPTH_COMPONENT16:
  case Texture2DInternalFormat::DEPTH_COMPONENT24:
  case Texture2DInternalFormat::DEPTH_COMPONENT32F:
    return Texture2DFormat::DEPTH_COMPONENT;

  case Texture2DInternalFormat::DEPTH24_STENCIL8:
  case Texture2DInternalFormat::DEPTH32F_STENCIL8:
    return Texture2DFormat::DEPTH_STENCIL;

  default:
    CONSOLE_WARN("Unknown internal format: {:#x} - Defaulting to Texture2DFormat::RED",
                 static_cast<u32>(internalFormat));
    
    return Texture2DFormat::RED;
  }
}
i32 Texture2D::GetNumChannels() const
{
  Texture2DInternalFormat internalFormat = GetInternalFormat();

  switch (internalFormat)
  {
  case Texture2DInternalFormat::R8:
  case Texture2DInternalFormat::R8_SNORM:
  case Texture2DInternalFormat::R16:
  case Texture2DInternalFormat::R16_SNORM:
  case Texture2DInternalFormat::R16F:
  case Texture2DInternalFormat::R32F:
    return 1;

  case Texture2DInternalFormat::RG8:
  case Texture2DInternalFormat::RG8_SNORM:
  case Texture2DInternalFormat::RG16:
  case Texture2DInternalFormat::RG16_SNORM:
  case Texture2DInternalFormat::RG16F:
  case Texture2DInternalFormat::RG32F:
    return 2;

  case Texture2DInternalFormat::RGB8:
  case Texture2DInternalFormat::RGB8_SNORM:
  case Texture2DInternalFormat::RGB16:
  case Texture2DInternalFormat::RGB16_SNORM:
  case Texture2DInternalFormat::RGB16F:
  case Texture2DInternalFormat::RGB32F:
  case Texture2DInternalFormat::R11F_G11F_B10F:
  case Texture2DInternalFormat::RGB9_E5:
  case Texture2DInternalFormat::SRGB8:
    return 3;

  case Texture2DInternalFormat::RGBA8:
  case Texture2DInternalFormat::RGBA8_SNORM:
  case Texture2DInternalFormat::RGBA16:
  case Texture2DInternalFormat::RGBA16_SNORM:
  case Texture2DInternalFormat::RGBA16F:
  case Texture2DInternalFormat::RGBA32F:
  case Texture2DInternalFormat::SRGB8_ALPHA8:
    return 4;

  case Texture2DInternalFormat::DEPTH_COMPONENT16:
  case Texture2DInternalFormat::DEPTH_COMPONENT24:
  case Texture2DInternalFormat::DEPTH_COMPONENT32F:
    return 1;

  case Texture2DInternalFormat::DEPTH24_STENCIL8:
  case Texture2DInternalFormat::DEPTH32F_STENCIL8:
    return 2;

  default:
    CONSOLE_WARN("Unknown internal format: {:#x} - Defaulting to 1 channel",
                 static_cast<u32>(internalFormat));
    return 1;
  }
}
