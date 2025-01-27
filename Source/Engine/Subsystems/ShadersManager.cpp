#include "ShadersManager.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Globals.hpp"
#include "Engine/IniFileHandler.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Utils.hpp"

constexpr char SM_FILE_CONFIG[] = "Shader_Manager.ini";

// --------------------------------------------
//                  PUBLIC                     
// --------------------------------------------

void ShadersManager::Initialize()
{
  CONSOLE_INFO("Loading shaders and programs...");
  
  IniFileHandler conf((Filesystem::GetRootPath() / SM_FILE_CONFIG));
  conf.ReadData();

  u32 nrShadersInDir = Utils::CountFilesInDirectory(Filesystem::GetShadersPath());
  u32 nrPrograms = conf.GetData().size();
  _shaders.reserve(nrShadersInDir);
  _programs.reserve(nrPrograms);

  ReadConfig(conf);
  
  SetProgramsUniforms();
}
void ShadersManager::CleanUp()
{
  // Destoy all program objects
  for (auto& program : _programs)
    program.Delete();

  // Destoy all shaders objects 
  for (auto& shader : _shaders)
    shader.Delete();
}

const Shader& ShadersManager::GetShader(StringView shaderName) const
{
  auto it = _shaderMap.find(shaderName.data());
  if (it == _shaderMap.end())
    throw std::runtime_error(std::format("Shader '{}' does not exist", shaderName.data()));
  
  u32 i = it->second;
  return _shaders.at(i);
}
const Shader& ShadersManager::GetOrInsertShader(StringView shaderName)
{
  auto it = _shaderMap.find(shaderName.data());
  if (it != _shaderMap.end())
  {
    u32 i = it->second;
    return _shaders.at(i);
  }
  return InsertShader(shaderName);
}
const Shader& ShadersManager::InsertShader(StringView shaderName)
{
  const fs::path& shadersDir = Filesystem::GetShadersPath();
  fs::path filePath = shadersDir / shaderName.data();
  
  if (!fs::exists(filePath) || !fs::is_regular_file(filePath))
    throw std::runtime_error(std::format("Shader file '{}' does not exist", filePath.string()));

  CONSOLE_TRACE("Loading shader '{}'", shaderName.data());

  u32 shaderIdx = _shaderMap.size();
  auto [it, success] = _shaderMap.emplace(
    std::piecewise_construct, 
    std::forward_as_tuple(shaderName.data()),
    std::forward_as_tuple(shaderIdx)
  );
  if (!success)
    throw std::runtime_error("Error on insert shader into the map");

  StringView ext = shaderName.substr(shaderName.find_last_of('.') + 1);
  i32 shaderType = ResolveShaderType(ext);

  IStream file(filePath);
  String shaderSrc{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

  Shader& shader = _shaders.emplace_back(shaderName);
  shader.Create(shaderType, shaderSrc);
  if (!shader.Compile())
    CONSOLE_ERROR("Error on compiling shader: {}", shader.GetShaderInfo());

  return shader;
}

const Program& ShadersManager::GetProgram(StringView programName) const
{
  auto it = _programMap.find(programName.data());
  if (it == _programMap.end())
    throw std::runtime_error(std::format("Program '{}' does not exist", programName.data()));
  
  u32 i = it->second;
  return _programs.at(i);
}
const Program& ShadersManager::InsertProgram(StringView programName)
{
  CONSOLE_TRACE("Loading new program '{}'", programName.data());
  
  u32 progIdx = _programMap.size();
  auto [it, success] = _programMap.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(programName.data()),
    std::forward_as_tuple(progIdx)
  );
  if (!success)
    throw std::runtime_error(std::format("Error on loading program into the map"));

  Program& program = _programs.emplace_back(programName);
  program.Create();
  return program;
}

// --------------------------------------------
//                  PRIVATE                    
// --------------------------------------------

i32 ShadersManager::ResolveShaderType(StringView ext)
{
  // GL_VERTEX_SHADER
  // GL_TESS_CONTROL_SHADER
  // GL_TESS_EVALUATION_SHADER
  // GL_GEOMETRY_SHADER 
  // GL_FRAGMENT_SHADER

  if (ext == "vert")
    return GL_VERTEX_SHADER;
  if (ext == "tesc")
    return GL_TESS_CONTROL_SHADER;
  if (ext == "tese")
    return GL_TESS_EVALUATION_SHADER;
  if (ext == "geom")
    return GL_GEOMETRY_SHADER;
  if (ext == "frag")
    return GL_FRAGMENT_SHADER;
  
  CONSOLE_WARN("Unknown file extension {}", ext.data());
  return 0;
}
void ShadersManager::ReadConfig(IniFileHandler& conf)
{
  conf.GetData().size();
  
  for (auto const& it : conf.GetData())
  {
    const String& section = it.first; // The program name 
    
    const Program& program = InsertProgram(section);
    if (!program.IsValid())
      continue;

    const String& vertex = conf.GetValue(section, "vertex");
    const String& tesc = conf.GetValue(section, "tess_control");
    const String& tese = conf.GetValue(section, "tess_eval");
    const String& geometry = conf.GetValue(section, "geometry");
    const String& fragment = conf.GetValue(section, "fragment");
    if (!vertex.empty())
    {
      const Shader& vertShader = GetOrInsertShader(vertex);
      program.AttachShader(vertShader);
    }
    if (!tesc.empty())
    {
      const Shader& tescShader = GetOrInsertShader(tesc);
      program.AttachShader(tescShader);
    }
    if (!tese.empty())
    {
      const Shader& teseShader = GetOrInsertShader(tese);
      program.AttachShader(teseShader);
    }
    if (!geometry.empty())
    {
      const Shader& geomShader = GetOrInsertShader(geometry);
      program.AttachShader(geomShader);
    }
    if (!fragment.empty())
    {
      const Shader& fragShader = GetOrInsertShader(fragment);
      program.AttachShader(fragShader);
    }

    if(!program.Link())
      CONSOLE_ERROR("Error on linking program '{}': {}", program.name, program.GetProgramInfo());
  }
}
void ShadersManager::SetProgramsUniforms() const
{
  auto& skyboxProg = GetProgram("Skybox");
  skyboxProg.SetUniform1i("u_skyboxTexture", 0);

  auto& framebufferProg = GetProgram("Framebuffer");
  framebufferProg.SetUniform1i("u_fboImageTexture", 0);
  framebufferProg.SetUniform1i("u_postProcessingType", 0);

  auto& sceneProg = GetProgram("Scene");
  sceneProg.SetUniform1i("u_useNormalMap", 0);
  sceneProg.SetUniform1i("u_material.diffuseTexture", 0);
  sceneProg.SetUniform1i("u_material.specularTexture", 1);
  sceneProg.SetUniform1i("u_material.normalTexture", 2);

  auto& sceneShadowsProg = GetProgram("SceneShadows");
  sceneShadowsProg.SetUniform1i("u_useNormalMap", 0);
  sceneShadowsProg.SetUniform1i("u_material.diffuseTexture", 0);
  sceneShadowsProg.SetUniform1i("u_material.specularTexture", 1);
  sceneShadowsProg.SetUniform1i("u_material.normalTexture", 2);
  sceneShadowsProg.SetUniform1i("u_depthMapTexture", 10);
  sceneShadowsProg.SetUniform1i("u_depthCubeMapTexture", 11);
  
  auto& skeletalAnimProg = GetProgram("SkeletalAnim");
  skeletalAnimProg.SetUniform1i("u_useNormalMap", 0);
  skeletalAnimProg.SetUniform1i("u_material.diffuseTexture", 0);
  skeletalAnimProg.SetUniform1i("u_material.specularTexture", 1);
  skeletalAnimProg.SetUniform1i("u_material.normalTexture", 2);
  for (u32 i = 0; i < 100; i++)
  {
    char uniform[32]{};
    std::format_to_n(uniform, sizeof(uniform), "u_boneTransforms[{}]", i);
    skeletalAnimProg.SetUniformMat4f(uniform, mat4f(1.0f));
  }
}