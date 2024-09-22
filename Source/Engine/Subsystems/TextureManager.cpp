#include "TextureManager.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

static void CreateDefaultTexture(Texture2D& texture, Array<u8, 3> textureData, StringView defaultPath)
{
  texture.Create();
  texture.CreateStorage(GL_RGB8, 1, 1);
  texture.UpdateStorage(0, 0, 0, GL_UNSIGNED_BYTE, textureData.data());
  texture.SetParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
  texture.SetParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
  texture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  texture.strPath = defaultPath.data();
}

/* -----------------------------------------------------  */
/*                    PUBLIC                              */
/* -----------------------------------------------------  */

void TextureManager::Initialize()
{
  /* 1. Create and load defaults */
  Texture2D defDiffuse(GL_TEXTURE_2D);
  Texture2D defSpecular(GL_TEXTURE_2D);
  Texture2D defNormal(GL_TEXTURE_2D);
  Texture2D defHeight(GL_TEXTURE_2D);
  CreateDefaultTexture(defDiffuse, { 128, 128, 255 }, "#default_diffuse");
  CreateDefaultTexture(defSpecular, { 255, 255, 255 }, "#default_specular");
  CreateDefaultTexture(defNormal, { 0, 0, 0 }, "#default_normal");
  CreateDefaultTexture(defHeight, { 0, 0, 0 }, "#default_height");
  _textures.emplace("#default_diffuse", defDiffuse);
  _textures.emplace("#default_specular", defSpecular);
  _textures.emplace("#default_normal", defNormal);
  _textures.emplace("#default_height", defHeight);

  /* 2. Load all textures from default Textures directory */
  CONSOLE_INFO("Loading textures...");
  LoadTextures();

  /* 3. Load all textures from default Icons directory */
  CONSOLE_INFO("Loading icons...");
  LoadIcons();
}
void TextureManager::CleanUp()
{
  u64 total = _textures.size() + _icons.size();
  
  Vector<u32> texIDs;
  texIDs.reserve(total);

  for (const auto& [key, texture] : _textures)
    texIDs.push_back(texture.id);
  
  for (const auto& [key, icon] : _icons)
    texIDs.push_back(icon.id);

  glDeleteTextures(total, texIDs.data());
}

Texture2D& TextureManager::GetTextureByPath(const fs::path& path)
{
  const String lexNormal = path.lexically_normal().string();

  const auto& it = _textures.find(lexNormal);
  if (it == _textures.end())
    throw std::runtime_error(std::format("Texture '{}' does not exist", lexNormal));
  
  return it->second;
}
Texture2D& TextureManager::GetIconByPath(const fs::path& path)
{
  const String lexNormal = path.lexically_normal().string();

  const auto& it = _icons.find(lexNormal);
  if (it == _icons.end())
    throw std::runtime_error(std::format("Icon '{}' does not exist", lexNormal));

  return it->second;
}

/* -----------------------------------------------------  */
/*                    PRIVATE                             */
/* -----------------------------------------------------  */

void TextureManager::LoadTextures()
{
  for (auto& entry : fs::recursive_directory_iterator(GetTexturesPath()))
  {
    if (fs::is_directory(entry))
      continue;

    const fs::path& entryPath = entry.path();
    bool gamma = false;
    const String filename = entryPath.filename().string();
    if (filename.find("diff") != String::npos || filename.find("diffuse") != String::npos)
      gamma = true;

    const String entryPathString = entryPath.string();
    auto res = _textures.emplace(entryPathString, Texture2D(GL_TEXTURE_2D, entryPathString, gamma));
    if (res.second)
      CONSOLE_TRACE("<{}> has been loaded successfully", entryPathString);
    else
      CONSOLE_WARN("Error on loading texture {}", entryPathString);
  }
}
void TextureManager::LoadIcons()
{
  for (auto& entry : fs::recursive_directory_iterator(GetIconsPath()))
  {
    if (fs::is_directory(entry))
      continue;

    const auto& entryPath = entry.path();
    const String entryPathString = entryPath.string();
    auto res = _icons.emplace(entryPathString, Texture2D(GL_TEXTURE_2D, entryPathString, false));
    if (res.second)
      CONSOLE_TRACE("<{}> has been loaded successfully", entryPathString);
    else
      CONSOLE_WARN("Error on loading icon <{}>", entryPathString);
  }
}

