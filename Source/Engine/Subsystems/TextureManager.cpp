#include "TextureManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/OpenGL.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void TextureManager::LoadTexturesFromDir(const fs::path& dirpath)
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
      const fs::path path = entry.path().lexically_normal();

      bool gamma = false;
      string filename = path.filename().string();
      
      if (filename.find("diff") != string::npos || filename.find("diffuse") != string::npos)
        gamma = true;

      auto& texture = LoadTexture(GL_TEXTURE_2D, path, gamma);
      CONSOLE_INFO("Texture loaded {}", path.string());
    }
  }
}

void TextureManager::CleanUp()
{
  uint64_t total = _textures.size() + _icons.size();
  if (total > 0)
  {
    vector<uint32_t> texIDs;
    texIDs.reserve(total);

    std::transform(_textures.begin(), _textures.end(), std::back_inserter(texIDs), [](const Texture2D& texture) {
      return texture.id;
    });
    std::transform(_icons.begin(), _icons.end(), std::back_inserter(texIDs), [](const Texture2D& texture) {
      return texture.id;
    });

    glDeleteTextures(total, texIDs.data());
  }

  vector<Texture2D>().swap(_textures);
  vector<Texture2D>().swap(_icons);
}

Texture2D& TextureManager::LoadTexture(int target, const fs::path& filePath, bool gammaCorrection)
{
  if (!fs::exists(filePath))
    CONSOLE_WARN("Texture '{}' does not exists", filePath.string());

  auto& texture = _textures.emplace_back(target, filePath, gammaCorrection);
  return texture;
}

Texture2D& TextureManager::LoadTextureIcon(int target, const fs::path& filePath)
{
  if (!fs::exists(filePath))
    CONSOLE_WARN("Texture '{}' does not exists", filePath.string());

  auto& icon = _icons.emplace_back(target, filePath, false);
  return icon;
}

Texture2D* TextureManager::GetTextureByPath(const fs::path& filePath)
{
  for (auto& text : _textures)
    if (text.path.compare(filePath) == 0)
      return &text;

  CONSOLE_WARN("Texture '{}' not found", filePath.string());
  return nullptr;
}

Texture2D* TextureManager::GetIconByPath(const fs::path& filePath)
{
  for (auto& icon: _icons)
    if (icon.path.compare(filePath) == 0)
      return &icon;

  CONSOLE_WARN("Texture '{}' not found", filePath.string());
  return nullptr;
}
