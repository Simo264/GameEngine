#include "TexturePool.hh"

#include <spdlog/spdlog.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

array<Texture2D, 10>	TexturePool::_textureBuffer;
uint32_t	            TexturePool::_numTextures = 0;

void TexturePool::Init()
{
  auto texturesDir = std::filesystem::current_path(); // VC++ solution path
  texturesDir = texturesDir.parent_path();
  texturesDir = texturesDir / "Textures";

  // load all shader files in Shaders directory
  for (const auto& entry : std::filesystem::directory_iterator(texturesDir))
    LoadTexture(entry);
}

Texture2D* TexturePool::LoadTexture(std::filesystem::path textureFilePath)
{
  if (_numTextures >= _textureBuffer.size())
  {
    spdlog::warn("Can't load more textures. Buffer is full");
    throw std::runtime_error("");
  }
  
  Texture2D* texture = &_textureBuffer[_numTextures++];
  texture->Init(textureFilePath);
  return texture;
}

Texture2D* TexturePool::GetTextureByName(const char* filename)
{
  for (uint32_t i = 0; i < _numTextures; i++)
  {
    auto texture = &_textureBuffer[i];
    auto texFilename = texture->texturePath.filename().string();

    if (texFilename.compare(filename) == 0)
      return texture;
  }
  return nullptr;
}
