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

Texture2D* TexturesManager::LoadTexture(Path filePath, bool gammaCorrection)
{
  if (_nTextures >= TEXTURES_MANAGER_MAX_SIZE)
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

  Texture2D* texture = &_textureBuffer[_nTextures++];
  texture->InitTexture(filePath, gammaCorrection);
  return texture;
}

Texture2D* TexturesManager::GetTexture(Path filePath)
{
  filePath = _texturesDir / filePath.lexically_normal();

  auto start = &_textureBuffer[0];
  auto end = &_textureBuffer[0] + _nTextures;
  auto it = std::find_if(start, end, [&filePath](Texture2D& texture) {
    return texture.texturePath.compare(filePath) == 0;
    });
  
  if (it == end)
    return nullptr;
  return it;
}

void TexturesManager::GetTextures(Vector<Texture2D*>& out)
{
  out.reserve(_nTextures);
  std::for_each_n(&_textureBuffer[0], _nTextures, [&out](Texture2D& ptr) {
    out.push_back(&ptr);
    });
}

void TexturesManager::ShutDown()
{
  std::for_each_n(&_textureBuffer[0], _nTextures, [](Texture2D& texture) {
    texture.DestroyTexture();
    });
  _textureBuffer.reset();
}
