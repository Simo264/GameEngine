#include "TextureManager.hpp"
#include "Engine/Graphics/Texture2D.hpp"

#include "Core/Log/Logger.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void TextureManager::Initialize()
{
  /* Preallocate memory for 32 textures */
  _textures.reserve(32);
}

void TextureManager::CleanUp()
{
	/* Destoy each texture objects */
  for (Texture2D* texture : _textures)
  {
    texture->Destroy();
    delete texture;
  }
}

Texture2D* TextureManager::LoadTexture(const Path& filePath, bool gammaCorrection)
{
  if (!std::filesystem::exists(filePath))
  {
    CONSOLE_WARN("File '{}' does not exists", filePath.string());
    return nullptr;
  }

  Texture2D* texture = new Texture2D(filePath, gammaCorrection);
  _textures.push_back(texture);
  return texture;
}

Texture2D* TextureManager::GetTextureByPath(const Path& filePath) const
{
  auto begin = _textures.begin();
  auto end = _textures.end();
  auto it = std::find_if(begin, end, [&filePath](Texture2D* texture) {
    return texture->texturePath.compare(filePath) == 0;
    });
  
  if (it == end)
  {
    CONSOLE_WARN("Texture '{}' does not exixts", filePath.string());
    return nullptr;
  }
  
  return *it;
}

Texture2D* TextureManager::Begin()
{
  return *_textures.begin();
}

Texture2D* TextureManager::End()
{
  return *_textures.end();
}
