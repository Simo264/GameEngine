#include "Texture2D.hpp"
#include "Engine/Graphics/Core/GL_Core.hpp"
#include "Core/Log/Logger.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Texture2D::Texture2D(Path path, bool gammaCorrection)
  : textureID{ 0 }
{
  texturePath = path;

  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  LoadImageData(gammaCorrection);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Destroy() const 
{ 
  glDeleteTextures(1, &textureID); 
}

void Texture2D::Bind() const 
{
  glBindTexture(GL_TEXTURE_2D, textureID); 
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Texture2D::LoadImageData(bool gammaCorrection) const
{
  String stringPath = texturePath.string();
  
  //stbi_set_flip_vertically_on_load(true);

  int width, height, nrChannels;
  auto data = stbi_load(stringPath.c_str(), &width, &height, &nrChannels, 0);
  if (data)
  {
    int internalFormat = GL_RGB;
    int format = GL_RGB;

    /*
      GL_RGB no gamma correction, no alpha component
      GL_RGBA no gamma correction, with alpha component
      GL_SRGB with gamma correction, no alpha component
      GL_SRGB_ALPHA with gamma correction, with alpha component    
    */
    switch (nrChannels) {
    case 1: 
      internalFormat = format = GL_RED;
      break;
    case 2: 
      internalFormat = format = GL_RG;
      break;
    case 3: 
      internalFormat = (gammaCorrection ? GL_SRGB : GL_RGB);
      format = GL_RGB; 
      break;
    case 4: 
      internalFormat = (gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA);
      format = GL_RGBA; 
      break;
    }

    /* Mutable storage */
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    CONSOLE_ERROR("Failed to load texture `{}`", stringPath);
  }

  stbi_image_free(reinterpret_cast<void*>(data));
}
