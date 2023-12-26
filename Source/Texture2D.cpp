#include "Texture2D.hh"
#include "Logger.hh"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Texture2D::Create(std::filesystem::path path)
{
  texturePath = path;

  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  LoadImageData();

  glBindTexture(GL_TEXTURE_2D, 0);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Texture2D::LoadImageData()
{
  string stringPath = texturePath.string();
  
  //stbi_set_flip_vertically_on_load(true);

  int width, height, nrChannels;
  auto data = stbi_load(stringPath.c_str(), &width, &height, &nrChannels, 0);
  if (data)
  {
    int format = GL_RGB;

    switch (nrChannels) {
      case 1: format = GL_RED;  break;
      case 2: format = GL_RG;   break;
      case 3: format = GL_RGB;  break;
      case 4: format = GL_RGBA; break;
    }

    // mutable storage
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    CONSOLE_ERROR("Failed to load texture `{}`", stringPath);
  }

  stbi_image_free(reinterpret_cast<void*>(data));
}
