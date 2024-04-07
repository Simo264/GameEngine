#include "Texture2D.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Texture2D::Texture2D()
  : id{ static_cast<uint32_t>(-1) },
    path{},
    mipmapLevels{ 0 },
    width{ 0 },
    height{ 0 },
    format{ 0 },
    internalformat{ 0 },
    type{ GL_UNSIGNED_BYTE }  /* default type value */
{}

Texture2D::Texture2D(const Path& path, bool gammaCorrection)
  : path{},
    mipmapLevels{ 0 },
    width{ 0 },
    height{ 0 },
    format{ 0 },              
    internalformat{ 0 },
    type{ GL_UNSIGNED_BYTE }  /* default type value */
{
  Create();

  LoadImageData(path, gammaCorrection);
}

void Texture2D::Create()
{
  glCreateTextures(GL_TEXTURE_2D, 1, &id);
}

void Texture2D::Delete() 
{ 
  glDeleteTextures(1, &id); 

  /* Invalidate texture object */
  id = static_cast<uint32_t>(-1); 
}

void Texture2D::Bind() const 
{
  glBindTexture(GL_TEXTURE_2D, id); 
}

void Texture2D::Unbind() const
{
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::BindTextureUnit(int unit) const
{
  glBindTextureUnit(unit, id);
}

void Texture2D::SetParameteri(int name, int value) const
{
  glTextureParameteri(id, name, value);
}

void Texture2D::GenerateMipmap() const
{
  glGenerateTextureMipmap(id);
}

void Texture2D::CreateStorage(int width, int height)
{
  this->width   = width;
  this->height  = height;
  mipmapLevels  = 1 + std::floor(std::log2(std::max(width, height)));

  if (internalformat == 0)
    CONSOLE_WARN("Invalid texture internalformat");

  glTextureStorage2D(id, mipmapLevels, internalformat, width, height);
}

void Texture2D::UpdateStorage(int level, int xoffset, int yoffset, const void* pixels) const
{
  if (format == 0)
    CONSOLE_WARN("Invalid texture format");

  glTextureSubImage2D(id, level, xoffset, yoffset, width, height, format, type, pixels);
}

void Texture2D::ClearStorage(int level, const void* data) const
{
  glClearTexImage(id, level, format, type, data);
}

void Texture2D::CopyStorage(int level, const Texture2D& dest) const
{
  if (!dest.IsValid())
    CONSOLE_WARN("Invalid destination texture object!");
  
  if (dest.width == 0 || dest.height == 0)
    CONSOLE_WARN("The destination texture storage is not allocated!");

  glCopyImageSubData(id, GL_TEXTURE_2D, level, 0, 0, 0, dest.id, GL_TEXTURE_2D, 0, 0, 0, 0, width, height, 1);
}

void Texture2D::LoadImageData(const Path& path, bool gammaCorrection)
{
  this->path = path;

  const String stringPath = path.string();
  
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
    switch (nrChannels) {
    case 1:
      internalformat  = GL_R8;
      format          = GL_RED;
      break;
    case 2:
      internalformat  = GL_RG8;
      format          = GL_RG;
      break;
    case 3:
      internalformat  = (gammaCorrection ? GL_SRGB8 : GL_RGB8);
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
    UpdateStorage(0, 0, 0, data);

    GenerateMipmap();
  }
  else
  {
    CONSOLE_ERROR("Failed to load texture `{}`", stringPath);
  }

  stbi_image_free(reinterpret_cast<void*>(data));
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/
