#include "ShadersManager.hh"
#include "../Logger.hh"

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

  const Path shadersDir = ROOT_PATH / "Shaders";
  auto testingShader = LoadShaderProgram("TestingShader",
    shadersDir / "Testing.vert",
    shadersDir / "Testing.frag");
  auto instancingShader = LoadShaderProgram(
    "InstancingShader",
    shadersDir / "Instancing.vert",
    shadersDir / "Scene.frag");
  auto sceneShader = LoadShaderProgram(
    "SceneShader",
    shadersDir / "Scene.vert",
    shadersDir / "Scene.frag");
  auto framebufferShader = LoadShaderProgram(
    "FramebufferShader",
    shadersDir / "Framebuffer.vert",
    shadersDir / "Framebuffer.frag");
  auto shadowMapDepthShader = LoadShaderProgram(
    "ShadowMapDepthShader",
    shadersDir / "ShadowMapDepth.vert",
    shadersDir / "ShadowMapDepth.frag");
  auto shadowMapShader = LoadShaderProgram(
    "ShadowMapShader",
    shadersDir / "ShadowMap.vert",
    shadersDir / "ShadowMap.frag");

  framebufferShader->Use();
  framebufferShader->SetInt("UScreenTexture", 0);
  framebufferShader->SetInt("UPostProcessingType", 0);

  instancingShader->Use();
  instancingShader->SetInt("UMaterial.diffuse", 0);
  instancingShader->SetInt("UMaterial.specular", 1);
  instancingShader->SetFloat("UMaterial.shininess", 32.0f);
  instancingShader->SetFloat("UGamma", 2.2f);

  sceneShader->Use();
  sceneShader->SetInt("UMaterial.diffuse", 0);
  sceneShader->SetInt("UMaterial.specular", 1);
  sceneShader->SetFloat("UMaterial.shininess", 32.0f);
  sceneShader->SetFloat("UGamma", 2.2f);
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

