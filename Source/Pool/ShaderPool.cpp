#include "ShaderPool.hh"

#include <spdlog/spdlog.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/
vector<std::filesystem::path> ShaderPool::_shaderFilePaths;
array<Graphics::Shader, 10>	  ShaderPool::_shaderBuffer;
uint32_t                      ShaderPool::_numShaders;

void ShaderPool::Init()
{
  // load all shader files in Shaders directory
  auto shadersDir = GetShadersDir();
  for (const auto& entry : std::filesystem::directory_iterator(shadersDir))
    _shaderFilePaths.push_back(entry.path());

  // init counter to 0
  _numShaders = 0;
}

std::filesystem::path ShaderPool::GetShaderFilePathByName(const char* filename)
{
  auto shaderFile = std::find_if(_shaderFilePaths.begin(), _shaderFilePaths.end(),
    [&](std::filesystem::path& path) { return path.filename().compare(filename) == 0; });

  if (shaderFile == _shaderFilePaths.end())
  {
    spdlog::error("Exception in GetShaderPathByName: {} not found", filename);
    throw std::runtime_error("");
  }

  return *shaderFile;
}

std::filesystem::path ShaderPool::GetShadersDir()
{
  auto rootPath = std::filesystem::current_path().parent_path();
  for (auto const& dirEntry : std::filesystem::directory_iterator{ rootPath })
  {
    auto dirName = dirEntry.path().filename();
    if (dirName.compare("Shaders") == 0)
      return dirEntry.path();
  }

  spdlog::error("RootPath={} Shaders dir not found", rootPath.string());
  throw std::runtime_error("");
}

Graphics::Shader* ShaderPool::LoadShader(const char* label,
  std::filesystem::path vertFilePath, 
  std::filesystem::path fragFilePath
)
{
  if (_numShaders >= _shaderBuffer.size())
  {
    spdlog::warn("Can't load more shaders. Buffer is full");
    throw std::runtime_error("");
  }

  Graphics::Shader* shader = &_shaderBuffer[_numShaders++];
  shader->Init(label, vertFilePath.string().c_str(), fragFilePath.string().c_str());
  return shader;
}


