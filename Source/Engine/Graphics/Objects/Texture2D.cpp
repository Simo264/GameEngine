#include "Texture2D.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Utils.hpp"

Texture2D::Texture2D(i32 target, StringView strPath, bool gammaCorrection)
  : id{ 0 },
    internalFormat{ 0 },
    format{ 0 },
    nChannels{ 0 },
    width{ 0 },
    height{ 0 },
    strPath{}
{
  Create(target);
  LoadImageData(strPath, gammaCorrection);
}

void Texture2D::Create(i32 target)
{
  if (target != GL_TEXTURE_2D && target != GL_TEXTURE_2D_MULTISAMPLE)
    CONSOLE_WARN("Invalid Texture2D target");

  this->target = target;
  glCreateTextures(target, 1, &id);
}
void Texture2D::Delete() 
{ 
  glDeleteTextures(1, &id); 
  id = 0;
}
void Texture2D::Bind() const 
{
  glBindTexture(target, id);
}
void Texture2D::Unbind() const
{
  glBindTexture(target, 0);
}
void Texture2D::BindTextureUnit(i32 unit) const
{
  glBindTextureUnit(unit, id);
}
void Texture2D::GenerateMipmap() const
{
  glGenerateTextureMipmap(id);
}

void Texture2D::CreateStorage(i32 internalFormat, i32 width, i32 height)
{
  this->width = width;
  this->height = height;
  this->internalFormat = internalFormat;
  i32 mipmapLevels = 1 + std::floor(std::log2(std::max(width, height)));

  glTextureStorage2D(id, mipmapLevels, internalFormat, width, height);
}
void Texture2D::CreateStorageMultisampled(i32 internalFormat, i32 samples, i32 width, i32 height)
{
  this->internalFormat = internalFormat;
  this->width = width;
  this->height = height;

  glTextureStorage2DMultisample(id, samples, internalFormat, width, height, true);
}
void Texture2D::UpdateStorage(i32 level, i32 xoffset, i32 yoffset, i32 type, const void* pixels) const
{
  glTextureSubImage2D(id, level, xoffset, yoffset, width, height, format, type, pixels);
}
void Texture2D::ClearStorage(i32 level, i32 type, const void* data) const
{
  glClearTexImage(id, level, format, type, data);
}

void Texture2D::LoadImageData(StringView strPath, bool gammaCorrection)
{
  i32 width, height, nChannels, internalFormat{ GL_RGB8 };
  u8* data = Utils::LoadImageData(strPath, width, height, nChannels);
  if (data)
  {
    /**
     * From https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
     * 
     * GL_RGB:         gamma correction: no;   alpha component: no
     * GL_RGBA:        gamma correction: no;   alpha component: yes
     * GL_SRGB:        gamma correction: yes;  alpha component: no 
     * GL_SRGB_ALPHA:  gamma correction: yes;  alpha component: yes   
     */

    switch (nChannels) 
    {
    case 1:
      internalFormat = GL_R8;
      this->format   = GL_RED;
      break;
    case 2:
      internalFormat = GL_RG8;
      this->format   = GL_RG;
      break;
    case 3:
      internalFormat = (gammaCorrection ? GL_SRGB8 : GL_RGB8);
      this->format   = GL_RGB;
      break;
    case 4:
      internalFormat = (gammaCorrection ? GL_SRGB8_ALPHA8 : GL_RGBA8);
      this->format   = GL_RGBA;
      break;
    }

    this->nChannels = nChannels;
    this->strPath = strPath.data();

    SetParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
    SetParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
    SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    CreateStorage(internalFormat, width, height);
    UpdateStorage(0, 0, 0, GL_UNSIGNED_BYTE, data);
    GenerateMipmap();
  }
  else
    CONSOLE_ERROR("Failed to load image data {}", strPath);

  Utils::FreeImageData(data);
}

void Texture2D::GetTextureImage(i32 level, i32 type, i32 bufSize, void* pixels) const
{
  glGetTextureImage(id, level, format, type, bufSize, pixels);
}

void Texture2D::SetParameteri(i32 name, i32 value) const
{
  glTextureParameteri(id, name, value);
}
void Texture2D::SetParameteriv(i32 name, i32* values) const
{
  glTextureParameteriv(id, name, values);
}
void Texture2D::SetParameterf(i32 name, f32 value) const
{
  glTextureParameterf(id, name, value);
}
void Texture2D::SetParameterfv(i32 name, f32* values) const
{
  glTextureParameterfv(id, name, values);
}
