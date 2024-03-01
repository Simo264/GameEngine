#include "ShaderManager.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Core/Log/Logger.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void ShaderManager::Initialize()
{
  /* Preallocate memory for 16 shaders */
  _shaders.reserve(16); 
}

void ShaderManager::CleanUp()
{
  /* Destoy all shaders */
  for (Shader* shader : _shaders)
  {
    shader->DestroyShader();
    delete shader;
  }
}

Shader* ShaderManager::LoadShader(const char* label, const Path& vertFilePath, const Path& fragFilePath)
{
  Shader* shader = new Shader(label,vertFilePath,fragFilePath);
  _shaders.push_back(shader);
  return shader;
}

Shader* ShaderManager::GetShader(const char* label) const
{
  auto beg = _shaders.begin();
  auto end = _shaders.end();
  auto it = std::find_if(beg, end, [&label](Shader* shader) {
      return shader->GetLabel().compare(label) == 0;
    });
  
  if (it == end)
    return nullptr;
  return *it;
}

