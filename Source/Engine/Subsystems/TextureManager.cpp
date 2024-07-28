#include "TextureManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/OpenGL.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void TextureManager::LoadTexturesFromDir(const fspath& dirpath)
{
  if (!fs::exists(dirpath) || !fs::is_directory(dirpath))
  {
    CONSOLE_WARN("<dirpath> is not a valid path", dirpath.string().c_str());
    return;
  }

  for (auto& entry : fs::recursive_directory_iterator(dirpath))
  {
    if (!fs::is_directory(entry))
    {
      const fspath path = entry.path().lexically_normal();
      CONSOLE_TRACE("Loading texture: {}", path.string().c_str());
      LoadTexture(path);
    }
  }
}

void TextureManager::CleanUp()
{
  for (auto& text : textures)
    text.Delete();

  for (auto& text : icons)
    text.Delete();

  textures.clear();
  icons.clear();
}

Texture2D* TextureManager::LoadTexture(const fspath& filePath, bool gammaCorrection)
{
  if (!std::filesystem::exists(filePath))
  {
    CONSOLE_WARN("Texture '{}' does not exists", filePath.string());
    return nullptr;
  }

  auto& texture = textures.emplace_back(GL_TEXTURE_2D, filePath, gammaCorrection);
  return &texture;
}

Texture2D* TextureManager::LoadTextureIcon(const fspath& filePath)
{
  if (!std::filesystem::exists(filePath))
  {
    CONSOLE_WARN("Texture '{}' does not exists", filePath.string());
    return nullptr;
  }

  auto& icon = icons.emplace_back(GL_TEXTURE_2D, filePath, false);
  return &icon;
}

Texture2D* TextureManager::GetTextureByPath(const fspath& filePath)
{
  for (auto& text : textures)
    if (text.path.compare(filePath) == 0)
      return &text;

  CONSOLE_WARN("Texture '{}' not found", filePath.string());
  return nullptr;
}

Texture2D* TextureManager::GetIconByPath(const fspath& filePath)
{
  for (auto& icon: icons)
    if (icon.path.compare(filePath) == 0)
      return &icon;

  CONSOLE_WARN("Texture '{}' not found", filePath.string());
  return nullptr;
}
