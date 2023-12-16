#include "ShadersManager.hh"

#include <spdlog/spdlog.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

array<Graphics::Shader, 10>	  ShadersManager::_shaderProgramsBuffer;
uint32_t                      ShadersManager::_nShaderPrograms;

std::filesystem::path         ShadersManager::_shadersDir;

void ShadersManager::Init()
{
  _shadersDir = std::filesystem::current_path(); // VC++ solution path
  _shadersDir = _shadersDir.parent_path();
  _shadersDir = _shadersDir / "Shaders";

  // init number of shaders to 0
  _nShaderPrograms = 0;
}

std::filesystem::path ShadersManager::GetShaderFileByName(const char* filename)
{
  std::filesystem::path shaderPath = _shadersDir / filename;

  if (!std::filesystem::exists(shaderPath))
  {
    spdlog::error("Exception in GetShaderPathByName: {} not exists", shaderPath.string());
    throw std::runtime_error("");
  }

  return shaderPath;
}

Graphics::Shader* ShadersManager::LoadShaderProgram(const char* label,
  std::filesystem::path vertFilePath, 
  std::filesystem::path fragFilePath
)
{
  if (_nShaderPrograms >= _shaderProgramsBuffer.size())
  {
    spdlog::warn("Can't load more shaders. Buffer is full");
    throw std::runtime_error("");
  }

  Graphics::Shader* shader = &_shaderProgramsBuffer[_nShaderPrograms++];
  shader->Init(label, vertFilePath.string().c_str(), fragFilePath.string().c_str());
  return shader;
}

Graphics::Shader* ShadersManager::GetShaderProgramByLabel(const string& label)
{
  for (uint32_t i = 0; i < _nShaderPrograms; i++)
  {
    auto* shader = &_shaderProgramsBuffer[i];
    if (shader->Label().compare(label) == 0)
      return shader;
  }
  return nullptr;
}
