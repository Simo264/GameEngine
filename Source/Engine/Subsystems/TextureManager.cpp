#include "TextureManager.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"


/* -----------------------------------------------------  */
/*                    PUBLIC                              */
/* -----------------------------------------------------  */

void TextureManager::Initialize()
{
  /* 1. Load textures */
  LoadTextures();

  /* 2. Load icons */
  LoadIcons();
}

void TextureManager::CleanUp()
{
  u64 total = _textures.size() + _icons.size();
  if (total > 0)
  {
    Vector<u32> texIDs;
    texIDs.reserve(total);

    std::transform(_textures.begin(), _textures.end(), std::back_inserter(texIDs), [](const Texture2D& texture) {
      return texture.id;
    });
    std::transform(_icons.begin(), _icons.end(), std::back_inserter(texIDs), [](const Texture2D& icon) {
      return icon.id;
    });

    glDeleteTextures(total, texIDs.data());
  }

  Vector<Texture2D>().swap(_textures);
  Vector<Texture2D>().swap(_icons);
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


/* -----------------------------------------------------  */
/*                    PRIVATE                             */
/* -----------------------------------------------------  */

void TextureManager::LoadTextures()
{
  /* The first 4 positions are reserved for the default textures */
  Texture2D defDiffuseTexture(GL_TEXTURE_2D);
  Texture2D defSpecularTexture(GL_TEXTURE_2D);
  Texture2D defNormalTexture(GL_TEXTURE_2D);
  Texture2D defHeightTexture(GL_TEXTURE_2D);
  CreateDefaultTexture(defDiffuseTexture, { 128, 128, 255 }, "#default_diffuse");
  CreateDefaultTexture(defSpecularTexture, { 255, 255, 255 }, "#default_specular");
  CreateDefaultTexture(defNormalTexture, { 0, 0, 0 }, "#default_normal");
  CreateDefaultTexture(defHeightTexture, { 0, 0, 0 }, "#default_height");

  _textures.push_back(defDiffuseTexture);
  _textures.push_back(defSpecularTexture);
  _textures.push_back(defNormalTexture);
  _textures.push_back(defHeightTexture);


  /* Load all from default Textures directory */
  for (auto& entry : fs::recursive_directory_iterator(TEXTURES_PATH))
  {
    if (fs::is_directory(entry))
      continue;

    const fs::path& path = entry.path();
    String filename = path.filename().string();
    bool gamma = false;
    if (filename.find("diff") != String::npos || filename.find("diffuse") != String::npos)
      gamma = true;

    auto& texture = _textures.emplace_back(GL_TEXTURE_2D, path.lexically_normal(), gamma);
    CONSOLE_TRACE("Texture loaded {}", path.string());
  }
}
void TextureManager::LoadIcons()
{
  /* Load all from default Icons directory */
  for (auto& entry : fs::recursive_directory_iterator(ICONS_PATH))
  {
    if (fs::is_directory(entry))
      continue;

    const auto& path = entry.path();
    auto& icon = _icons.emplace_back(GL_TEXTURE_2D, path.lexically_normal(), false);
    CONSOLE_TRACE("Icon loaded {}", path.string());
  }
}
void TextureManager::CreateDefaultTexture(Texture2D& texture, Array<u8, 3> textureData, StringView defaultPath)
{
  texture.Create();
  texture.CreateStorage(GL_RGB8, 1, 1);
  texture.UpdateStorage(0, 0, 0, GL_UNSIGNED_BYTE, textureData.data());
  texture.SetParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
  texture.SetParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
  texture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  texture.path = fs::path(defaultPath.data());
}
