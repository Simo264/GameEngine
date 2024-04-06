#include "Texture2D.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Texture2D::Texture2D(const Path& path, bool gammaCorrection)
  : path{ path },
    width{ 0 },
    height{ 0 },
    format{ GL_RGB },         /* default format value */
    internalformat{ GL_RGB }, /* default internalformat value */
    type{ GL_UNSIGNED_BYTE }  /* default type value */
{
  Create();

  SetParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
  SetParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
  SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  LoadImageData(path, gammaCorrection);
}

void Texture2D::Create()
{
  glCreateTextures(GL_TEXTURE_2D, 1, &id);
}

void Texture2D::Delete() const 
{ 
  glDeleteTextures(1, &id); 
}

void Texture2D::Bind() const 
{
  glBindTexture(GL_TEXTURE_2D, id); 
}

void Texture2D::Unbind() const
{
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::SetParameteri(int name, int value)
{
  glTextureParameteri(id, name, value);
}

void Texture2D::GenerateMipmap() const
{
  glGenerateTextureMipmap(id);
}

void Texture2D::CreateStorage(int levels, int internalformat, int width, int height)
{
  glTextureStorage2D(id, levels, internalformat, width, height);
  this->width = width;
  this->height = height;
  this->internalformat = internalformat;
}

void Texture2D::UpdateStorage(int level, int xoffset, int yoffset, const void* pixels) const
{
  glTextureSubImage2D(id, level, xoffset, yoffset, width, height, format, type, pixels);
}

void Texture2D::Copy()
{
  // https://registry.khronos.org/OpenGL/extensions/ARB/ARB_direct_state_access.txt
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Texture2D::LoadImageData(const Path& path, bool gammaCorrection)
{
  const String stringPath = path.string();
  
  //stbi_set_flip_vertically_on_load(true);

  int width, height, nrChannels;
  auto data = stbi_load(stringPath.c_str(), &width, &height, &nrChannels, 0);
  if (data)
  {
    int internalformat  = GL_RGB;

    /*
      GL_RGB:         no gamma correction, no alpha component
      GL_RGBA:        no gamma correction, with alpha component
      GL_SRGB:        with gamma correction, no alpha component
      GL_SRGB_ALPHA:  with gamma correction, with alpha component    
    */
    switch (nrChannels) {
    case 1: 
      internalformat  = GL_RED;
      format          = GL_RED;
      break;
    case 2: 
      internalformat  = GL_RG;
      format          = GL_RG;
      break;
    case 3: 
      internalformat  = (gammaCorrection ? GL_SRGB : GL_RGB);
      format          = GL_RGB; 
      break;
    case 4: 
      internalformat  = (gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA);
      format          = GL_RGBA; 
      break;
    }

    //glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
    CreateStorage(0, internalformat, width, height);
    UpdateStorage(0, 0, 0, data);

    GenerateMipmap();
  }
  else
  {
    CONSOLE_ERROR("Failed to load texture `{}`", stringPath);
  }

  stbi_image_free(reinterpret_cast<void*>(data));
}
