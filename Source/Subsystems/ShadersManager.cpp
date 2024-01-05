#include "ShadersManager.hh"
#include "../Logger.hh"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Path ShadersManager::GetShaderFile(Path filePath)
{
  filePath = _shadersDir / filePath.lexically_normal();

  if (!std::filesystem::exists(filePath))
  {
    CONSOLE_ERROR("Exception in GetShaderPathByName: {} not exists", filePath.string());
    throw RuntimeError("");
  }

  return filePath;
}

Shader* ShadersManager::LoadShaderProgram(const char* label, Path vertFilePath, Path fragFilePath)
{
  if (_nShaderPrograms >= _shaderProgramsBuffer.size())
  {
    CONSOLE_WARN("Can't load more shaders. Buffer is full");
    throw RuntimeError("");
  }

  Shader* shader = &_shaderProgramsBuffer[_nShaderPrograms++];
  shader->InitShader(label, vertFilePath.string().c_str(), fragFilePath.string().c_str());
  return shader;
}

Shader* ShadersManager::GetShader(const char* label)
{
  for (uint32_t i = 0; i < _nShaderPrograms; i++)
  {
    Shader* shader = &_shaderProgramsBuffer[i];
    if (shader->Label().compare(label) == 0)
      return shader;
  }
  return nullptr;
}
