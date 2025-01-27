#include "Texture2D.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Utils.hpp"

Texture2D::Texture2D(Texture2DTarget target, const fs::path& imagePath, bool gammaCorrection)
  : id{ 0 },
    internalFormat{ 0 },
    format{ 0 },
    nChannels{ 0 },
    width{ 0 },
    height{ 0 },
    path{}
{
  Create(target);
  LoadImageData(imagePath, gammaCorrection);
}

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

void Texture2D::CreateStorage(Texture2DInternalFormat internalFormat, i32 width, i32 height)
{
  this->width = width;
  this->height = height;
  this->internalFormat = internalFormat;
  u32 mipmapLevels = 1 + std::floor(std::log2(std::max(width, height)));

  glTextureStorage2D(
    id, 
    mipmapLevels, 
    static_cast<u32>(internalFormat),
    width, 
    height);
}
void Texture2D::CreateStorageMultisampled(Texture2DInternalFormat internalFormat, i32 samples, i32 width, i32 height)
{
  this->internalFormat = internalFormat;
  this->width = width;
  this->height = height;

  glTextureStorage2DMultisample(
    id, 
    samples, 
    static_cast<u32>(internalFormat),
    width, 
    height, 
    true);
}
void Texture2D::UpdateStorage(i32 level, i32 xoffset, i32 yoffset, i32 type, const void* pixels) const
{
  glTextureSubImage2D(id, level, xoffset, yoffset, width, height, format, type, pixels);
}
void Texture2D::ClearStorage(i32 level, i32 type, const void* data) const
{
  glClearTexImage(id, level, format, type, data);
}

void Texture2D::LoadImageData(const fs::path& imagePath, bool gammaCorrection)
{
  i32 width, height, nChannels;
  Texture2DInternalFormat internalFormat = Texture2DInternalFormat::RGB8;
  u8* data = Utils::LoadImageData(imagePath, width, height, nChannels);
  if (data)
  {
    // From https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
    //
    // GL_RGB:         gamma correction: no;   alpha component: no
    // GL_RGBA:        gamma correction: no;   alpha component: yes
    // GL_SRGB:        gamma correction: yes;  alpha component: no 
    // GL_SRGB_ALPHA:  gamma correction: yes;  alpha component: yes   

    switch (nChannels) 
    {
    case 1:
      internalFormat = Texture2DInternalFormat::R8;
      this->format   = GL_RED;
      break;
    case 2:
      internalFormat = Texture2DInternalFormat::RG8;
      this->format   = GL_RG;
      break;
    case 3:
      internalFormat = (gammaCorrection ? Texture2DInternalFormat::SRGB8: Texture2DInternalFormat::RGB8);
      this->format   = GL_RGB;
      break;
    case 4:
      internalFormat = (gammaCorrection ? Texture2DInternalFormat::SRGB8_ALPHA8 : Texture2DInternalFormat::RGBA8);
      this->format   = GL_RGBA;
      break;
    }

    this->nChannels = nChannels;
    this->path = imagePath;

    SetParameteri(TextureParameteriName::TextureWrapS, TextureParameteriParam::Repeat);
    SetParameteri(TextureParameteriName::TextureWrapT, TextureParameteriParam::Repeat);
    SetParameteri(TextureParameteriName::TextureMinFilter, TextureParameteriParam::LinearMipmapLinear);
    SetParameteri(TextureParameteriName::TextureMagFilter, TextureParameteriParam::Linear);

    CreateStorage(internalFormat, width, height);
    UpdateStorage(0, 0, 0, GL_UNSIGNED_BYTE, data);
    GenerateMipmap();
  }
  else
    CONSOLE_ERROR("Failed to load image data {}", imagePath.string());

  Utils::FreeImageData(data);
}

void Texture2D::GetTextureImage(i32 level, i32 type, i32 buffSize, void* pixels) const
{
  glGetTextureImage(id, level, format, type, buffSize, pixels);
}

void Texture2D::SetParameteri(TextureParameteriName name, TextureParameteriParam value) const
{
  glTextureParameteri(
    id, 
    static_cast<u32>(name),
    static_cast<i32>(value));
}

void Texture2D::SetParameterfv(TextureParameteriName name, f32* values) const
{
  glTextureParameterfv(
    id,
    static_cast<u32>(name),
    values);
}

void Texture2D::SetCompareFunc(CompareFunc func) const
{
  glTextureParameteri(
    id, 
    static_cast<u32>(TextureParameteriName::TextureCompareFunc), 
    static_cast<i32>(func));
}
