#include "TexturesManager.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Utils.hpp"

static void CreateDefaultTexture(Texture2D& texture, u8 r, u8 g, u8 b)
{
  constexpr i32 width = 1;
  constexpr i32 height = 1;
  
  auto data = Array<u8, 3>{ r,g,b }.data();
  texture.Create(Texture2DTarget::TEXTURE_2D);
  texture.CreateStorage(Texture2DInternalFormat::RGB8, width, height);
  texture.UpdateStorage(0, width, height, Texture2DFormat::RGB, Texture2DSubImageType::UNSIGNED_BYTE, data);
  texture.SetParameteri(TextureParameteriName::WRAP_S, TextureParameteriParam::REPEAT);
  texture.SetParameteri(TextureParameteriName::WRAP_T, TextureParameteriParam::REPEAT);
  texture.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::LINEAR);
  texture.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::LINEAR);
}

// ----------------------------------------------------- 
//                    PUBLIC                             
// -----------------------------------------------------

void TexturesManager::Initialize()
{
  u32 nrTextures = Utils::CountFilesInDirectory(Filesystem::GetTexturesPath(), true);
  u32 nrIcons = Utils::CountFilesInDirectory(Filesystem::GetIconsPath(), true);
  _textures.reserve(nrTextures + 3);
  _icons.reserve(nrIcons);

  Texture2D& diffuse = _textures.emplace_back();
  CreateDefaultTexture(diffuse, 128, 128, 255);
  Texture2D& specular = _textures.emplace_back();
  CreateDefaultTexture(specular, 0, 0, 0);
  Texture2D& normal = _textures.emplace_back();
  CreateDefaultTexture(normal, 0, 0, 0);
}
void TexturesManager::CleanUp()
{
  u32 totalTextures = _textures.size() + _icons.size();

  Vector<u32> texIDs;
  texIDs.reserve(totalTextures);

  for(const auto& texture : _textures)
    texIDs.push_back(texture.id);
  for (const auto& texture : _icons)
    texIDs.push_back(texture.id);

  glDeleteTextures(totalTextures, texIDs.data());
}

const Texture2D* TexturesManager::FindTexture(const fs::path& relative) const
{
  auto it = _textureMap.find(relative);
  if (it == _textureMap.end())
    return nullptr;

  u32 index = it->second;
  return &_textures.at(index);
}
const Texture2D* TexturesManager::CreateTexture(const fs::path& relative)
{
  CONSOLE_TRACE("Insert new texture: {}", relative.lexically_normal().string());
  u32 newIndex = _textures.size();
  auto [it, success] = _textureMap.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(relative.lexically_normal()),
    std::forward_as_tuple(newIndex)
  );
  if (!success)
  {
    CONSOLE_ERROR("Error on insert texture into the map");
    return nullptr;
  }
  
  fs::path absolute = (Filesystem::GetTexturesPath() / relative).lexically_normal();
  CONSOLE_INFO("Create new texture: {}", absolute.string());

  Texture2D& texture = _textures.emplace_back();
  texture.Create(Texture2DTarget::TEXTURE_2D);
  texture.LoadImageData(absolute);
  return &texture;
}
const Texture2D* TexturesManager::GetOrCreateTexture(const fs::path& relative)
{
  const auto* t = FindTexture(relative);
  if (!t)
    t = CreateTexture(relative);
  return t;
}

const Texture2D* TexturesManager::FindIcon(const fs::path& relative) const
{
  auto it = _iconMap.find(relative);
  if (it == _iconMap.end())
    return nullptr;

  u32 index = it->second;
  return &_icons.at(index);
}
const Texture2D* TexturesManager::CreateIcon(const fs::path& relative)
{
  CONSOLE_TRACE("Insert new icon: {}", relative.lexically_normal().string());
  u32 newIndex = _icons.size();
  auto [it, success] = _iconMap.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(relative.lexically_normal()),
    std::forward_as_tuple(newIndex)
  );
  if (!success)
  {
    CONSOLE_ERROR("Error on insert texture icon into the map");
    return nullptr;
  }

  fs::path absolutePath = (Filesystem::GetIconsPath() / relative).lexically_normal();
  CONSOLE_INFO("Create new icon: {}", absolutePath.string());

  Texture2D& icon = _icons.emplace_back();
  icon.Create(Texture2DTarget::TEXTURE_2D);
  icon.LoadImageData(absolutePath);
  return &icon;
}
const Texture2D* TexturesManager::GetOrCreateIcon(const fs::path& relative)
{
  const auto* t = FindIcon(relative);
  if (!t)
    t = CreateIcon(relative);
  return t;
}

// ----------------------------------------------------- 
//                    PRIVATE                            
// -----------------------------------------------------

