#include "TexturesManager.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Paths/Paths.hpp"
#include "Engine/Utils.hpp"
#include "Engine/Importers/ImageLoader.hpp"

static Texture2D CreateDefaultTexture(u8 r, u8 g, u8 b)
{
  constexpr auto width = 1;
  constexpr auto height = 1;
  const auto data = Array<u8, 3>{ r, g, b }.data();

  auto texture = Texture2D();
  texture.Create(Texture2DTarget::TEXTURE_2D);
  texture.CreateStorage(Texture2DInternalFormat::RGB8, width, height);
  texture.UpdateStorage(0, width, height, Texture2DSubImageType::UNSIGNED_BYTE, data);
  texture.SetParameteri(TextureParameteriName::WRAP_S, TextureParameteriParam::REPEAT);
  texture.SetParameteri(TextureParameteriName::WRAP_T, TextureParameteriParam::REPEAT);
  texture.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::LINEAR);
  texture.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::LINEAR);
  return texture;
}

// -----------------------------------------------------
//                    PUBLIC
// -----------------------------------------------------

void TexturesManager::Initialize()
{
  _defaultAlbedo = CreateDefaultTexture(128, 128, 255);
  _defaultNormal = CreateDefaultTexture(0, 0, 0);
}
void TexturesManager::Cleanup()
{
  auto totalTextures = _textures.size() + _icons.size();

  auto texIDs = Vector<u32>{};
  texIDs.reserve(totalTextures + 3);

  texIDs.push_back(_defaultAlbedo.id);
  texIDs.push_back(_defaultNormal.id);

  for (const auto &texture : _textures)
    texIDs.push_back(texture.id);
  for (const auto &texture : _icons)
    texIDs.push_back(texture.id);

  glDeleteTextures(totalTextures, texIDs.data());
}

Texture2D TexturesManager::FindTexture(const fs::path &relative) const
{
  for (auto i = 0u; i < _texturePaths.size(); i++)
    if (_texturePaths.at(i) == relative)
      return _textures.at(i);

  return Texture2D{};
}
Texture2D TexturesManager::CreateTexture(const fs::path &relative)
{
  auto absolute = fs::path(Paths::GetTexturesPath() / relative).lexically_normal();
  CONSOLE_INFO("Create new texture: {}", absolute.string());
  
  auto loader = ImageLoader{};
  auto data = loader.LoadImageData(absolute);
  assert(data != nullptr);

  auto& texture = _textures.emplace_back();
  texture.Create(Texture2DTarget::TEXTURE_2D);
  texture.CreateStorage(static_cast<Texture2DInternalFormat>(loader.internalFormat), loader.width, loader.height);
  texture.UpdateStorage(0, loader.width, loader.height, Texture2DSubImageType::UNSIGNED_BYTE, data);
  texture.GenerateMipmap();
  texture.SetParameteri(TextureParameteriName::WRAP_S, TextureParameteriParam::REPEAT);
  texture.SetParameteri(TextureParameteriName::WRAP_T, TextureParameteriParam::REPEAT);
  texture.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::LINEAR);
  texture.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::LINEAR);

  loader.FreeImageData(data);

  auto normalized = relative.lexically_normal();
  _texturePaths.emplace_back(normalized);

  return texture;
}
Texture2D TexturesManager::GetOrCreateTexture(const fs::path &relative)
{
  auto t = FindTexture(relative);
  if (!t.IsValid())
    t = CreateTexture(relative);
  return t;
}

Texture2D TexturesManager::FindIcon(const fs::path &relative) const
{
  for (auto i = 0u; i < _iconPaths.size(); i++)
    if (_iconPaths.at(i) == relative)
      return _icons.at(i);
  return Texture2D{};
}
Texture2D TexturesManager::CreateIcon(const fs::path &relative)
{
  auto absolute = fs::path(Paths::GetIconsPath() / relative).lexically_normal();
  CONSOLE_INFO("Create new icon: {}", absolute.string());
  
  auto loader = ImageLoader{};
  auto data = loader.LoadImageData(absolute);
  assert(data != nullptr);
  
  auto& texture = _icons.emplace_back();
  texture.Create(Texture2DTarget::TEXTURE_2D);
  texture.CreateStorage(static_cast<Texture2DInternalFormat>(loader.internalFormat), loader.width, loader.height);
  texture.UpdateStorage(0, loader.width, loader.height, Texture2DSubImageType::UNSIGNED_BYTE, data);
  texture.GenerateMipmap();
  texture.SetParameteri(TextureParameteriName::WRAP_S, TextureParameteriParam::REPEAT);
  texture.SetParameteri(TextureParameteriName::WRAP_T, TextureParameteriParam::REPEAT);
  texture.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::LINEAR);
  texture.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::LINEAR);

  loader.FreeImageData(data);

  auto normalized = relative.lexically_normal();
  _iconPaths.emplace_back(normalized);
  return texture;
}
Texture2D TexturesManager::GetOrCreateIcon(const fs::path &relative)
{
  auto t = FindIcon(relative);
  if (!t.IsValid())
    t = CreateIcon(relative);
  return t;
}

const fs::path *TexturesManager::GetTexturePath(u32 textureID) const
{
  for (auto i = 0u; i < _textures.size(); i++)
    if (_textures.at(i).id == textureID)
      return &_texturePaths.at(i);
  return nullptr;
}
