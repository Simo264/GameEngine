#include "TextureManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/Platform/OpenGL/OpenGL.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void TextureManager::Initialize()
{
  _size = 0;
  _nTextures = 0;

  /* Calculate how many textures and icons must be loaded */
  for (auto& entry : std::filesystem::recursive_directory_iterator(TEXTURES_PATH))
    if (!std::filesystem::is_directory(entry))
      _size++;

  for (auto& entry : std::filesystem::recursive_directory_iterator(ICONS_PATH))
    if (!std::filesystem::is_directory(entry))
      _size++;

  assert(!(_size == 0) && "Cannot create the texture pool with size 0");

  _pool = UniquePointer<Texture2D[]>(new Texture2D[_size]);
}

void TextureManager::CleanUp()
{
  const auto beg = Begin();
  const auto end = End();

  std::for_each(beg, end, [](Texture2D& texture) {
    texture.Delete();
    });
  
  _pool.reset();
}

void TextureManager::LoadTextures()
{
  for (auto& entry : std::filesystem::recursive_directory_iterator(TEXTURES_PATH))
    if (!std::filesystem::is_directory(entry))
      LoadTexture(entry);
}

void TextureManager::LoadIcons()
{
  for (auto& entry : std::filesystem::recursive_directory_iterator(ICONS_PATH))
    if (!std::filesystem::is_directory(entry))
      LoadTexture(entry);
}

Texture2D* TextureManager::LoadTexture(const Path& filePath, bool gammaCorrection)
{
  const Path normalPath = filePath.lexically_normal();
  if (!std::filesystem::exists(normalPath))
  {
    CONSOLE_WARN("Texture '{}' does not exists", filePath.string());
    return nullptr;
  }

  auto& texture = _pool[_nTextures++];
  texture.Create(GL_TEXTURE_2D);
  texture.LoadImageData(normalPath, true);
  return &texture;
}

Texture2D* TextureManager::GetTextureByPath(const Path& filePath)
{
  const Path normalPath = filePath.lexically_normal();
  for (auto it = Begin(); it != End(); ++it)
    if (it->path.compare(normalPath) == 0)
      return it;
  
  CONSOLE_WARN("Texture '{}' not found", normalPath.string());
  return nullptr;
}

