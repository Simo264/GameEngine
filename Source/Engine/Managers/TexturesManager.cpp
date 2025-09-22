#include "TexturesManager.hpp"

#include "Core/Logger.hpp"
#include "Engine/Utils.hpp"
#include "Engine/Importers/ImageLoader.hpp"

#include <glad/gl.h>

void TexturesManager::Initialize()
{
  constexpr auto width = 1;
  constexpr auto height = 1;
  constexpr auto rgb = Array<byte, 3>{ byte(128), byte(128), byte(255) };

  _fallbackTexture = Texture2D{};
  _fallbackTexture.Create(Texture2DTarget::TEXTURE_2D);
  _fallbackTexture.CreateStorage(Texture2DInternalFormat::RGB8, width, height);
  _fallbackTexture.UpdateStorage(0, width, height, Texture2DSubImageType::UNSIGNED_BYTE, rgb.data());
  _fallbackTexture.SetParameteri(TextureParameteriName::WRAP_S, TextureParameteriParam::REPEAT);
  _fallbackTexture.SetParameteri(TextureParameteriName::WRAP_T, TextureParameteriParam::REPEAT);
  _fallbackTexture.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::LINEAR);
  _fallbackTexture.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::LINEAR);
}
void TexturesManager::Cleanup()
{
  auto totalTextures = _textures.size() + _icons.size();
  auto texIDs = Vector<u32>{};
  texIDs.reserve(totalTextures + 1);

  texIDs.push_back(_fallbackTexture.id);
  for (const auto &texture : _textures)
    texIDs.push_back(texture.id);
  for (const auto &texture : _icons)
    texIDs.push_back(texture.id);

  glDeleteTextures(totalTextures, texIDs.data());
}

Optional<Texture2D> TexturesManager::FindTexture(const fs::path &relative) const
{
  for (auto i = 0u; i < _texturePaths.size(); i++)
    if (_texturePaths.at(i) == relative)
      return _textures.at(i);

  return std::nullopt;
}
Texture2D TexturesManager::CreateTexture(const fs::path &relative)
{
  auto absolute = fs::path(Utils::GetTexturesPath() / relative).lexically_normal();
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
  if (!t)
    t = CreateTexture(relative);
  return t.value();
}

Optional<Texture2D> TexturesManager::FindIcon(const fs::path &relative) const
{
  for (auto i = 0u; i < _iconPaths.size(); i++)
    if (_iconPaths.at(i) == relative)
      return _icons.at(i);
  return std::nullopt;
}

Texture2D TexturesManager::CreateIcon(const fs::path &relative)
{
  auto absolute = fs::path(Utils::GetIconsPath() / relative).lexically_normal();
  auto loader = ImageLoader{};
  auto data = loader.LoadImageData(absolute);
  
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
  if (!t)
    t = CreateIcon(relative);
  return t.value();
}

Optional<const fs::path*> TexturesManager::GetTexturePath(u32 textureID) const
{
  for (auto i = 0u; i < _textures.size(); i++)
    if (_textures.at(i).id == textureID)
      return &_texturePaths.at(i);
  
  return std::nullopt;
}
