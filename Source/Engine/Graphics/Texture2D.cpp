#include "Texture2D.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/OpenGL.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture2D::Texture2D()
  : id{ static_cast<uint32_t>(-1) },
    target{ 0 },
    path{},
    format{ 0 },
    internalformat{ 0 },
    _mipmapLevels{ 0 },
    _width{ 0 },
    _height{ 0 },
    _samples{ 0 },
    _isMultisampled{ false }
{}

Texture2D::Texture2D(int target, const fspath& path, bool gammaCorrection)
  : path{ },
    format{ 0 },
    internalformat{ 0 },
    _mipmapLevels{ 0 },
    _width{ 0 },
    _height{ 0 },
    _samples{ 0 },
    _isMultisampled{ false }
{
  Create(target);
  LoadImageData(path, gammaCorrection);
}

void Texture2D::Create(int target)
{
  if (target != GL_TEXTURE_2D && target != GL_TEXTURE_2D_MULTISAMPLE)
  {
    CONSOLE_ERROR("Invalid texture target");
    return;
  }

  this->target = target;
  glCreateTextures(target, 1, &id);
}
void Texture2D::Delete() 
{ 
  glDeleteTextures(1, &id); 
  id = static_cast<uint32_t>(-1); 
}
void Texture2D::Bind() const 
{
  glBindTexture(target, id);
}
void Texture2D::Unbind() const
{
  glBindTexture(target, 0);
}
void Texture2D::BindTextureUnit(int unit) const
{
  glBindTextureUnit(unit, id);
}
void Texture2D::GenerateMipmap() const
{
  glGenerateTextureMipmap(id);
}

void Texture2D::CreateStorage(int width, int height)
{
  if (internalformat == 0)
    CONSOLE_WARN("Invalid texure internalformat");
  
  _width          = width;
  _height         = height;
  _isMultisampled = false;
  _mipmapLevels   = 1 + std::floor(std::log2(std::max(width, height)));

  glTextureStorage2D(id, _mipmapLevels, internalformat, width, height);
}
void Texture2D::CreateStorageMultisampled(int samples, int width, int height, bool fixedsamplelocations)
{
  if (internalformat == 0)
    CONSOLE_WARN("Invalid texure internalformat");

  _width          = width;
  _height         = height;
  _samples        = samples;
  _isMultisampled = true;
  _mipmapLevels   = 1 + std::floor(std::log2(std::max(width, height)));
  
  glTextureStorage2DMultisample(id, samples, internalformat, width, height, fixedsamplelocations);
}
void Texture2D::UpdateStorage(int level, int xoffset, int yoffset, int type, const void* pixels) const
{
  if (internalformat == 0)
    CONSOLE_WARN("Invalid texure internalformat");

  glTextureSubImage2D(id, level, xoffset, yoffset, _width, _height, format, type, pixels);
}
void Texture2D::ClearStorage(int level, int type, const void* data) const
{
  if (format == 0)
    CONSOLE_WARN("Invalid texure format");

  glClearTexImage(id, level, format, type, data);
}
void Texture2D::CopyStorage(int level, const Texture2D& dest) const
{
  if (!dest.IsValid())
  {
    CONSOLE_WARN("Invalid destination texture object!");
    return;
  }
  if (dest.GetWidth() == 0 || dest.GetHeight() == 0)
  {
    CONSOLE_WARN("Invalid destination texture size!");
    return;
  }

  glCopyImageSubData(id, GL_TEXTURE_2D, level, 0, 0, 0, dest.id, GL_TEXTURE_2D, 0, 0, 0, 0, _width, _height, 1);
}

void Texture2D::LoadImageData(const fspath& path, bool gammaCorrection)
{
  this->path = path;

  const string stringPath = path.string();
  
  int width, height, nrChannels;
  auto data = stbi_load(stringPath.c_str(), &width, &height, &nrChannels, 0);
  if (data)
  {
    /*
      From https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
     
      GL_RGB:         no gamma correction, no alpha component
      GL_RGBA:        no gamma correction, with alpha component
      GL_SRGB:        with gamma correction, no alpha component
      GL_SRGB_ALPHA:  with gamma correction, with alpha component
    */
    switch (nrChannels) 
    {
    case 1:
      internalformat  = GL_R8;
      format          = GL_RED;
      break;
    case 2:
      internalformat  = GL_RG8;
      format          = GL_RG;
      break;
    case 3:
      internalformat = (gammaCorrection ? GL_SRGB8 : GL_RGB8);
      format          = GL_RGB;
      break;
    case 4:
      internalformat  = (gammaCorrection ? GL_SRGB8_ALPHA8 : GL_RGBA8);
      format          = GL_RGBA;
      break;
    }

    SetParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
    SetParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
    SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    CreateStorage(width, height);
    UpdateStorage(0, 0, 0, GL_UNSIGNED_BYTE, data);
    GenerateMipmap();
  }
  else
  {
    CONSOLE_ERROR("Failed to load texture `{}`", stringPath);
  }

  stbi_image_free(reinterpret_cast<void*>(data));
}

void Texture2D::SetParameteri(int name, int value) const
{
  glTextureParameteri(id, name, value);
}
void Texture2D::SetParameteriv(int name, int* values) const
{
  glTextureParameteriv(id, name, values);
}
void Texture2D::SetParameterf(int name, float value) const
{
  glTextureParameterf(id, name, value);
}
void Texture2D::SetParameterfv(int name, float* values) const
{
  glTextureParameterfv(id, name, values);
}
