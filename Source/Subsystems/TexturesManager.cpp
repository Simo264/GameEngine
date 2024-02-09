#include "TexturesManager.hh"
#include "../Logger.hh"

static constexpr uint32_t TEXTURES_MANAGER_MAX_SIZE = 100;

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void TexturesManager::Initialize()
{
	/* Reserve block of memory with TEXTURES_MANAGER_MAX_SIZE on the heap */
	_textureBuffer = std::make_unique<Texture2D[]>(TEXTURES_MANAGER_MAX_SIZE);
	_bufferSize = 0;
}

void TexturesManager::ShutDown()
{
	/* Destoy each texture objects */
	std::for_each(Begin(), End(), [](Texture2D& texture) {
    texture.DestroyTexture();
    });
	
	/* Deallocate memory */
  _textureBuffer.reset();
}

Texture2D* TexturesManager::LoadTexture(Path filePath, bool gammaCorrection)
{
  if (_bufferSize >= TEXTURES_MANAGER_MAX_SIZE)
  {
    CONSOLE_WARN("Can't load more textures. Buffer is full");
    return nullptr;
  }
  if (!std::filesystem::exists(filePath))
  {
    CONSOLE_WARN("Texture '{}' does not exists", filePath.string());
    return nullptr;
  }
  Texture2D& texture = _textureBuffer[_bufferSize++];
  texture.InitTexture(filePath, gammaCorrection);
  return &texture;
}

Texture2D* TexturesManager::GetTextureByPath(Path filePath) const
{
  auto end = End();
  auto it = std::find_if(Begin(), End(), [&filePath](Texture2D& texture) {
    return texture.texturePath.compare(filePath) == 0;
    });
  
  if (it == end)
    return nullptr;
  return it;
}

