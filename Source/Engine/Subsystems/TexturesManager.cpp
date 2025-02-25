#include "TexturesManager.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Utils.hpp"

static Texture2D CreateDefaultTexture(u8 r, u8 g, u8 b)
{
  constexpr i32 width = 1;
  constexpr i32 height = 1;
  const u8* data = Array<u8, 3>{ r,g,b }.data();
	
  Texture2D texture;
  texture.Create(Texture2DTarget::TEXTURE_2D);
  texture.CreateStorage(Texture2DInternalFormat::RGB8, width, height);
  texture.UpdateStorage(0, width, height, Texture2DFormat::RGB, Texture2DSubImageType::UNSIGNED_BYTE, data);
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
	_defaultDiffuse = CreateDefaultTexture(128, 128, 255);
  _defaultSpecular = CreateDefaultTexture(0, 0, 0);
  _defaultNormal = CreateDefaultTexture(0, 0, 0);
}
void TexturesManager::CleanUp()
{
  u32 totalTextures = _textures.size() + _icons.size();

  Vector<u32> texIDs;
  texIDs.reserve(totalTextures + 3);

	texIDs.push_back(_defaultDiffuse.id);
  texIDs.push_back(_defaultSpecular.id);
  texIDs.push_back(_defaultNormal.id);

  for(const auto& texture : _textures)
    texIDs.push_back(texture.id);
  for (const auto& texture : _icons)
    texIDs.push_back(texture.id);

  glDeleteTextures(totalTextures, texIDs.data());
}

Texture2D TexturesManager::FindTexture(const fs::path& relative) const
{
  for (u32 i = 0; i < _texturePaths.size(); i++)
    if (_texturePaths.at(i) == relative)
      return _textures.at(i);
  
  return Texture2D{};
}
Texture2D TexturesManager::CreateTexture(const fs::path& relative)
{
  fs::path absolute = (Filesystem::GetTexturesPath() / relative).lexically_normal();
  CONSOLE_INFO("Create new texture: {}", absolute.string());
  Texture2D& texture = _textures.emplace_back();
  texture.Create(Texture2DTarget::TEXTURE_2D);
  texture.LoadImageData(absolute);

  fs::path normalized = relative.lexically_normal();
  _texturePaths.emplace_back(normalized);

  return texture;
}
Texture2D TexturesManager::GetOrCreateTexture(const fs::path& relative)
{
  Texture2D t = FindTexture(relative);
  if (!t.IsValid())
    t = CreateTexture(relative);
  return t;
}

Texture2D TexturesManager::FindIcon(const fs::path& relative) const
{
  for (u32 i = 0; i < _iconPaths.size(); i++)
    if (_iconPaths.at(i) == relative)
      return _icons.at(i);
  return Texture2D{};
}
Texture2D TexturesManager::CreateIcon(const fs::path& relative)
{
  fs::path absolute = (Filesystem::GetIconsPath() / relative).lexically_normal();
  CONSOLE_INFO("Create new icon: {}", absolute.string());
  Texture2D& icon = _icons.emplace_back();
  icon.Create(Texture2DTarget::TEXTURE_2D);
  icon.LoadImageData(absolute);

  fs::path normalized = relative.lexically_normal();
  _iconPaths.emplace_back(normalized);
  return icon;
}
Texture2D TexturesManager::GetOrCreateIcon(const fs::path& relative)
{
  Texture2D t = FindIcon(relative);
  if (!t.IsValid())
    t = CreateIcon(relative);
  return t;
}

const const fs::path* TexturesManager::GetTexturePath(u32 textureID) const
{
  for (u32 i = 0; i < _textures.size(); i++)
    if(_textures.at(i).id == textureID)
			return &_texturePaths.at(i);
	return nullptr;
}
