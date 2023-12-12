#include "ShaderPool.hh"

#include <spdlog/spdlog.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/
vector<std::filesystem::path> ShaderPool::_shaderFiles;
array<Graphics::Shader, 10>	  ShaderPool::_shaderProgramsBuffer;
uint32_t                      ShaderPool::_nShaderPrograms;

void ShaderPool::Init()
{
  auto shadersDir = std::filesystem::current_path(); // VC++ solution path
  shadersDir = shadersDir.parent_path();
  shadersDir = shadersDir / "Shaders";

  // load all shader files in Shaders directory
  for (const auto& entry : std::filesystem::directory_iterator(shadersDir))
    _shaderFiles.push_back(entry.path());

  // init counter to 0
  _nShaderPrograms = 0;
}

std::filesystem::path ShaderPool::GetShaderFileByName(const char* filename)
{
  auto shaderFile = std::find_if(_shaderFiles.begin(), _shaderFiles.end(),
    [&](std::filesystem::path& path) { return path.filename().compare(filename) == 0; });

  if (shaderFile == _shaderFiles.end())
  {
    spdlog::error("Exception in GetShaderPathByName: {} not found", filename);
    throw std::runtime_error("");
  }

  return *shaderFile;
}

Graphics::Shader* ShaderPool::LoadShader(const char* label,
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

Graphics::Shader* ShaderPool::GetShaderByLabel(const string& label)
{
  for (uint32_t i = 0; i < _nShaderPrograms; i++)
  {
    auto* shader = &_shaderProgramsBuffer[i];
    if (shader->Label().compare(label) == 0)
      return shader;
  }
  return nullptr;
}
