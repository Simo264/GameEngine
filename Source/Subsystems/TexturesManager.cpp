#include "TexturesManager.hh"
#include "../Logger.hh"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void TexturesManager::Initialize()
{
  // Load all texture files in Textures directory
  for (const auto& entry : std::filesystem::recursive_directory_iterator(_texturesDir))
  {
    if (!std::filesystem::is_directory(entry))
    {
      LoadTexture(entry);
    }
  }
}

Texture2D* TexturesManager::LoadTexture(Path filePath)
{
  if (_numTextures >= _textureBuffer.size())
  {
    CONSOLE_ERROR("Can't load more textures. Buffer is full");
    throw RuntimeError("");
  }
  
  filePath = _texturesDir / filePath.lexically_normal();
  if (!std::filesystem::exists(filePath))
  {
    CONSOLE_ERROR("Exception in TexturesManager::LoadTexture: {} does not exists", filePath.string());
    throw RuntimeError("");
  }

  Texture2D* texture = &_textureBuffer[_numTextures++];
  texture->Create(filePath);
  return texture;
}

Texture2D* TexturesManager::GetTexture(Path filePath)
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

