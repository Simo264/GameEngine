#include "TexturesManager.hh"

#include <spdlog/spdlog.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/
std::filesystem::path TexturesManager::_texturesDir;
array<Texture2D, 10>	TexturesManager::_textureBuffer;
uint32_t	            TexturesManager::_numTextures = 0;

void TexturesManager::Init()
{
  _texturesDir = std::filesystem::current_path(); // VC++ solution path
  _texturesDir = _texturesDir.parent_path();
  _texturesDir = _texturesDir / "Textures";

  // load all texture files in Textures directory
  for (const auto& entry : std::filesystem::directory_iterator(_texturesDir))
    LoadTexture(entry);
}

Texture2D* TexturesManager::LoadTexture(std::filesystem::path textureFilePath)
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

Texture2D* TexturesManager::GetTextureByName(const char* filename)
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
