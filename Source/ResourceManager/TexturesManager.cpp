#include "TexturesManager.hh"

#include <spdlog/spdlog.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/
std::filesystem::path TexturesManager::_texturesDir;
array<Texture2D, 10>	TexturesManager::_textureBuffer;
uint32_t	            TexturesManager::_numTextures;

void TexturesManager::Init()
{
  _texturesDir = std::filesystem::current_path().parent_path() / "Textures"; 
  _numTextures = 0;

  // Load all texture files in Textures directory
  for (const auto& entry : std::filesystem::recursive_directory_iterator(_texturesDir))
  {
    if (!std::filesystem::is_directory(entry))
    {
      LoadTexture(entry);
    }
  }
}

Texture2D* TexturesManager::LoadTexture(std::filesystem::path filePath)
{
  if (_numTextures >= _textureBuffer.size())
  {
    spdlog::error("Can't load more textures. Buffer is full");
    throw std::runtime_error("");
  }
  
  filePath = _texturesDir / filePath.lexically_normal();
  if (!std::filesystem::exists(filePath))
  {
    spdlog::error("Exception in TexturesManager::LoadTexture: {} does not exists", filePath.string());
    throw std::runtime_error("");
  }

  Texture2D* texture = &_textureBuffer[_numTextures++];
  texture->Init(filePath);
  return texture;
}

Texture2D* TexturesManager::GetTexture(std::filesystem::path filePath)
{
  filePath = _texturesDir / filePath.lexically_normal();

  for (uint32_t i = 0; i < _numTextures; i++)
  {
    auto texture = &_textureBuffer[i];
    if (texture->texturePath.compare(filePath) == 0)
      return texture;
  }

  return nullptr;
}

