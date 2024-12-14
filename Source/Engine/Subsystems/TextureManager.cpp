#include "TextureManager.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

static void CreateDefaultTexture(Texture2D& texture, u8 r, u8 g, u8 b, StringView strPath)
{
  texture.format = GL_RGB;
  texture.Create(GL_TEXTURE_2D);
  texture.CreateStorage(GL_RGB8, 1, 1);
  texture.UpdateStorage(0, 0, 0, GL_UNSIGNED_BYTE, Array<u8, 3>{ r,g,b }.data());
  texture.SetParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
  texture.SetParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
  texture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  texture.strPath = String(strPath.data());
}

/* -----------------------------------------------------  */
/*                    PUBLIC                              */
/* -----------------------------------------------------  */

void TextureManager::Initialize()
{
  /* 1. Create and load defaults */
  Texture2D defDiffuse;
  Texture2D defSpecular;
  Texture2D defNormal;
  Texture2D defHeight;
  CreateDefaultTexture(defDiffuse, 128, 128, 255, "#default_diffuse");
  CreateDefaultTexture(defSpecular, 0, 0, 0, "#default_specular");
  CreateDefaultTexture(defNormal, 0, 0, 0, "#default_normal");
  CreateDefaultTexture(defHeight,0, 0, 0, "#default_height");
  _textures.emplace("#default_diffuse", defDiffuse);
  _textures.emplace("#default_specular", defSpecular);
  _textures.emplace("#default_normal", defNormal);
  _textures.emplace("#default_height", defHeight);

  /* 2. Load all textures */
  CONSOLE_INFO("Loading textures...");
  LoadTextures();
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
  try
  {
    return _textures.at(path);
  }
  catch (const std::exception&)
  {
    throw std::runtime_error(std::format("texture '{}' does not exist", path.string()));
  }
}
Texture2D& TextureManager::GetIconByPath(const fs::path& path)
{
  try
  {
    return _icons.at(path);
  }
  catch (const std::exception&)
  {
    throw std::runtime_error(std::format("texture '{}' does not exist", path.string()));
  }
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

    const auto& entryPath = entry.path();
    String entryPathString = entryPath.string();
    String filename = entryPath.filename().string();
    bool gamma = false;
    if (filename.find("diff") != String::npos)
      gamma = true;
    
    CONSOLE_TRACE("{}", entryPathString);
    auto emplaceResult = _textures.emplace(entryPathString.c_str(), Texture2D());
    bool success = emplaceResult.second;
    if (success)
    {
      auto& it = emplaceResult.first;
      Texture2D& texture = it->second;
      texture.Create(GL_TEXTURE_2D);
      texture.LoadImageData(entryPath, gamma);
    }
    else
      CONSOLE_ERROR("Error on loading texture object");
  }
}
void TextureManager::LoadIcons()
{
  for (auto& entry : fs::recursive_directory_iterator(GetIconsPath()))
  {
    if (fs::is_directory(entry))
      continue;

    const auto& entryPath = entry.path();
    String entryPathString = entryPath.string();
    
    CONSOLE_TRACE("{}", entryPathString);
    auto emplaceResult = _icons.emplace(entryPathString.c_str(), Texture2D());
    bool success = emplaceResult.second;
    if (success)
    {
      auto& it = emplaceResult.first;
      Texture2D& texture = it->second;
      texture.Create(GL_TEXTURE_2D);
      texture.LoadImageData(entryPath, false);
    }
    else
      CONSOLE_ERROR("Error on loading texture object");
  }
}

