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
  auto start = &_shaderProgramsBuffer[0];
  auto end = &_shaderProgramsBuffer[0] + _nShaderPrograms;
  auto it = std::find_if(start, end, [&label](Shader& shader) {
      return shader.Label().compare(label) == 0;
    });
  if (it == end)
    return nullptr;
  
  return it;
}

void ShadersManager::Initialize()
{
  LoadShaderProgram("TestingShader",GetShaderFile("Testing.vert"),GetShaderFile("Testing.frag"));
  auto instancingShader = LoadShaderProgram(
    "InstancingShader",
    GetShaderFile("Instancing.vert"),
    GetShaderFile("Scene.frag"));
  auto sceneShader = LoadShaderProgram(
    "SceneShader",
    GetShaderFile("Scene.vert"),
    GetShaderFile("Scene.frag"));
  auto framebufferShader = LoadShaderProgram(
    "FramebufferShader",
    GetShaderFile("Framebuffer.vert"),
    GetShaderFile("Framebuffer.frag"));

  framebufferShader->Use();
  framebufferShader->SetInt("screenTexture", 0);
  
  instancingShader->Use();
  instancingShader->SetInt("Material.diffuse", 0);
  instancingShader->SetInt("Material.specular", 1);
  instancingShader->SetFloat("Material.shininess", 32.0f);
  
  sceneShader->Use();
  sceneShader->SetInt("Material.diffuse", 0);
  sceneShader->SetInt("Material.specular", 1);
  sceneShader->SetFloat("Material.shininess", 32.0f);
}

void ShadersManager::ShutDown()
{
  std::for_each_n(&_shaderProgramsBuffer[0], _nShaderPrograms, [](Shader& shader) {
    shader.DestroyShader();
  });
}