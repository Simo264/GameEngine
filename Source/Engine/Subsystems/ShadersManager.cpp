#include "ShadersManager.hpp"
#include "Logger.hpp"

static constexpr uint32_t SHADERS_MANAGER_MAX_SIZE = 10;

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void ShadersManager::Initialize()
{
  /* Reserve block of memory with TEXTURES_MANAGER_MAX_SIZE on the heap */
  _shaderBuffer = std::make_unique<Shader[]>(SHADERS_MANAGER_MAX_SIZE);
  _bufferSize = 0;
}

void ShadersManager::ShutDown()
{
  /* Destoy each texture objects */
  std::for_each_n(&_shaderBuffer[0], _bufferSize, [](Shader& shader) {
    shader.DestroyShader();
    });
  
  /* Deallocate memory */
  _shaderBuffer.reset();
}

Shader* ShadersManager::LoadShaderProgram(const char* label, Path vertFilePath, Path fragFilePath)
{
  if (_bufferSize >= SHADERS_MANAGER_MAX_SIZE)
  {
    CONSOLE_WARN("Can't load more shaders. Buffer is full");
    return nullptr;
  }

  Shader& shader = _shaderBuffer[_bufferSize++];
  shader.InitShader(label, vertFilePath.string().c_str(), fragFilePath.string().c_str());
  return &shader;
}

Shader* ShadersManager::GetShader(const char* label)
{
  auto start = &_shaderBuffer[0];
  auto end = &_shaderBuffer[0] + _bufferSize;
  auto it = std::find_if(start, end, [&label](Shader& shader) {
      return shader.Label().compare(label) == 0;
    });
  
  if (it == end)
    return nullptr;
  return it;
}

