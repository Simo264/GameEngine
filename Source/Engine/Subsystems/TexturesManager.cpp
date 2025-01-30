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


  // Load all from Textures/ and Icons/
  CONSOLE_INFO("Loading textures and icons...");
  //LoadAllTextures();
  LoadAllIcons();
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

const Texture2D* TexturesManager::FindTexture(const fs::path& relativePath) const
{
  auto it = _textureMap.find(relativePath.lexically_normal());
  if (it == _textureMap.end())
    return nullptr;

  u32 index = it->second;
  return &_textures.at(index);
}
const Texture2D* TexturesManager::LoadTexture(const fs::path& relativePath, bool gamma)
{
  u32 newIndex = _textures.size();
  auto [it, success] = _textureMap.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(relativePath.lexically_normal()),
    std::forward_as_tuple(newIndex)
  );
  if (!success)
  {
    CONSOLE_ERROR("Error on insert texture into the map");
    return nullptr;
  }
  
  fs::path absolutePath = (Filesystem::GetTexturesPath() / relativePath).lexically_normal();
  Texture2D& texture = _textures.emplace_back();
  texture.Create(Texture2DTarget::TEXTURE_2D);
  texture.LoadImageData(absolutePath, gamma);
  return &texture;
}
const Texture2D* TexturesManager::GetOrLoadTexture(const fs::path& relativePath, bool gamma)
{
  const auto* t = FindTexture(relativePath);
  if (!t)
    t = LoadTexture(relativePath);
  return t;
}

const Texture2D* TexturesManager::FindTextureIcon(const fs::path& relativePath) const
{
  auto it = _iconMap.find(relativePath.lexically_normal());
  if (it == _iconMap.end())
    return nullptr;

  u32 index = it->second;
  return &_icons.at(index);
}
const Texture2D* TexturesManager::LoadTextureIcon(const fs::path& relativePath)
{
  u32 newIndex = _icons.size();
  auto [it, success] = _iconMap.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(relativePath.lexically_normal()),
    std::forward_as_tuple(newIndex)
  );
  if (!success)
  {
    CONSOLE_ERROR("Error on insert texture icon into the map");
    return nullptr;
  }

  fs::path absolutePath = (Filesystem::GetIconsPath() / relativePath).lexically_normal();
  Texture2D& icon = _icons.emplace_back();
  icon.Create(Texture2DTarget::TEXTURE_2D);
  icon.LoadImageData(absolutePath, false);
  return &icon;
}
const Texture2D* TexturesManager::GetOrLoadTextureIcon(const fs::path& relativePath, bool gamma)
{
  const auto* t = FindTextureIcon(relativePath);
  if (!t)
    t = LoadTextureIcon(relativePath);
  return t;
}

// ----------------------------------------------------- 
//                    PRIVATE                            
// -----------------------------------------------------

void TexturesManager::LoadAllTextures()
{
  for (auto& entry : fs::recursive_directory_iterator(Filesystem::GetTexturesPath()))
  {
    if (!fs::is_regular_file(entry))
      continue;
    
    const auto& absolutePath = entry.path();
    const auto relativePath = fs::relative(absolutePath, Filesystem::GetTexturesPath());
    String filename = relativePath.filename().string();
    
    std::transform(filename.begin(),
      filename.end(),
      filename.begin(),
      ::tolower
    );

    bool gamma = false;
    if (filename.find("diff") != String::npos)
      gamma = true;

    CONSOLE_TRACE("{}", absolutePath.relative_path().string());
    const Texture2D* texture = LoadTexture(relativePath, gamma);
    if (!texture)
      CONSOLE_ERROR("Error on loading texture object");
  }
}
void TexturesManager::LoadAllIcons()
{
  for (auto& entry : fs::recursive_directory_iterator(Filesystem::GetIconsPath()))
  {
    if (!fs::is_regular_file(entry))
      continue;

    const auto& absolutePath = entry.path();
    const auto relativePath = fs::relative(absolutePath, Filesystem::GetIconsPath());
    CONSOLE_TRACE("{}", absolutePath.relative_path().string());

    const Texture2D* icon = LoadTextureIcon(relativePath);
    if (!icon)
      CONSOLE_ERROR("Error on loading texture icon object");
  }
}

