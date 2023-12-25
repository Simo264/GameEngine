#include "ShadersManager.hh"
#include "../Logger.hh"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

array<Shader, 10>	  ShadersManager::_shaderProgramsBuffer;
uint32_t                      ShadersManager::_nShaderPrograms;

std::filesystem::path         ShadersManager::_shadersDir;

void ShadersManager::Initialize()
{
  _shadersDir = std::filesystem::current_path().parent_path() / "Shaders"; // VC++ solution path

  // Set number of shader programs to 0
  _nShaderPrograms = 0;
}

std::filesystem::path ShadersManager::GetShaderFile(std::filesystem::path filePath)
{
  filePath = _shadersDir / filePath.lexically_normal();

  if (!std::filesystem::exists(filePath))
  {
    CONSOLE_ERROR("Exception in GetShaderPathByName: {} not exists", filePath.string());
    throw std::runtime_error("");
  }

  return filePath;
}

Shader* ShadersManager::LoadShaderProgram(const char* label,
  std::filesystem::path vertFilePath, 
  std::filesystem::path fragFilePath
)
{
  if (_nShaderPrograms >= _shaderProgramsBuffer.size())
  {
    CONSOLE_WARN("Can't load more shaders. Buffer is full");
    throw std::runtime_error("");
  }

  Shader* shader = &_shaderProgramsBuffer[_nShaderPrograms++];
  shader->Init(label, vertFilePath.string().c_str(), fragFilePath.string().c_str());
  return shader;
}

Shader* ShadersManager::GetShader(const char* label)
{
  for (uint32_t i = 0; i < _nShaderPrograms; i++)
  {
    auto* shader = &_shaderProgramsBuffer[i];
    if (shader->Label().compare(label) == 0)
      return shader;
  }
  return nullptr;
}
