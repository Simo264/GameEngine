#include "TextureManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/Platform/OpenGL/OpenGL.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void TextureManager::Initialize()
{
  /* Load textures */
  for (auto& entry : std::filesystem::recursive_directory_iterator(TEXTURES_PATH))
    if (!std::filesystem::is_directory(entry))
      LoadTexture(entry);
  
  /* Load icons */
  for (auto& entry : std::filesystem::recursive_directory_iterator(ICONS_PATH))
    if (!std::filesystem::is_directory(entry))
      LoadTexture(entry);
}

void TextureManager::CleanUp()
{
  for (auto& text : textures)
    text.Delete();
}

Texture2D* TextureManager::LoadTexture(const fspath& filePath, bool gammaCorrection)
{
  const fspath normalPath = filePath.lexically_normal();
  if (!std::filesystem::exists(normalPath))
  {
    CONSOLE_WARN("Texture '{}' does not exists", normalPath.string());
    return nullptr;
  }

  auto& texture = textures.emplace_back(normalPath, gammaCorrection);
  return &texture;
}

Texture2D* TextureManager::GetTextureByPath(const fspath& filePath)
{
  const fspath normalPath = filePath.lexically_normal();
  for (auto& text : textures)
    if (text.path.compare(filePath) == 0)
      return &text;

  CONSOLE_WARN("Texture '{}' not found", normalPath.string());
  return nullptr;
}

