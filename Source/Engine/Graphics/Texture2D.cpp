#include "Texture2D.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/OpenGL.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture2D::Texture2D(int target)
  : id{ 0 },
    target{ target },
    path{},
    internalFormat{ GL_RGB8 },
    format{ GL_RGB },
    nrChannels{ 3 },
    width{ 0 },
    height{ 0 }
{}

Texture2D::Texture2D(int target, const fs::path& path, bool gammaCorrection)
  : id{ 0 },
    target{ target },
    path{},
    internalFormat{ GL_RGB8 },
    format{ GL_RGB },
    nrChannels{ 3 },
    width{ 0 },
    height{ 0 }
{
  Create();
  LoadImageData(path, gammaCorrection);
}

void Texture2D::Create()
{
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
void Texture2D::BindTextureUnit(int unit) const
{
  glBindTextureUnit(unit, id);
}
void Texture2D::GenerateMipmap() const
{
  glGenerateTextureMipmap(id);
}

void Texture2D::CreateStorage(int internalFormat, int width, int height)
{
  this->internalFormat = internalFormat;
  this->width = width;
  this->height = height;
  int mipmapLevels = 1 + std::floor(std::log2(std::max(width, height)));

  glTextureStorage2D(id, mipmapLevels, internalFormat, width, height);
}
void Texture2D::CreateStorageMultisampled(int internalFormat, int samples, int width, int height)
{
  this->internalFormat = internalFormat;
  this->width = width;
  this->height = height;

  glTextureStorage2DMultisample(id, samples, internalFormat, width, height, true);
}
void Texture2D::UpdateStorage(int level, int xoffset, int yoffset, int type, const void* pixels) const
{
  glTextureSubImage2D(id, level, xoffset, yoffset, width, height, format, type, pixels);
}
void Texture2D::ClearStorage(int level, int type, const void* data) const
{
  glClearTexImage(id, level, format, type, data);
}

void Texture2D::LoadImageData(const fs::path& path, bool gammaCorrection)
{
  const string stringPath = path.string();
  
  int width, height, nrChannels, internalFormat{ GL_RGB8 };
  auto* data = stbi_load(stringPath.c_str(), &width, &height, &nrChannels, 0);
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

    switch (nrChannels) 
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

    this->nrChannels = nrChannels;
    this->path = path;

    SetParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
    SetParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
    SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    CreateStorage(internalFormat, width, height);
    UpdateStorage(0, 0, 0, GL_UNSIGNED_BYTE, data);
    GenerateMipmap();
  }
  else
  {
    CONSOLE_ERROR("Failed to load texture {}", stringPath);
  }

  stbi_image_free(reinterpret_cast<void*>(data));
}

void Texture2D::GetTextureImage(int level, int type, int bufSize, void* pixels) const
{
  glGetTextureImage(id, level, format, type, bufSize, pixels);
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
