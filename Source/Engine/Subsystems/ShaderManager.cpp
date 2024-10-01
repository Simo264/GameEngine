#include "ShaderManager.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Filesystem/ConfigFile.hpp"

#include "Engine/Globals.hpp"

constexpr char SM_FILE_CONFIG[] = { "Shader_Manager.ini" };

/* -------------------------------------------- */
/*                  PUBLIC                      */
/* -------------------------------------------- */

void ShaderManager::Initialize()
{
  /* Load shader files */
  CONSOLE_INFO("Loading shader...");
  LoadShaders();
  CONSOLE_INFO("Compiling shader...");
  CompileShaders();

  /* Load and link programs and set uniforms */
  CONSOLE_INFO("Loading programs...");
  LoadPrograms();
  CONSOLE_INFO("Linking programs...");
  LinkPrograms();
  SetProgramsUniforms();
}
void ShaderManager::CleanUp()
{
  /* Destoy all program objects */
  for (auto& [key, program] : _programs)
    program.Delete();

  /* Destoy all shaders objects */
  for (auto& [key, shader]  : _shaders)
    shader.Delete();
}

Shader& ShaderManager::GetShaderByName(StringView filename)
{
  const auto& it = _shaders.find(filename.data());
  if (it == _shaders.end())
    throw std::runtime_error(std::format("Shader '{}' does not exist", filename));
  
  return it->second;
}
Program& ShaderManager::GetProgramByName(StringView name)
{
  const auto& it = _programs.find(name.data());
  if (it == _programs.end())
    throw std::runtime_error(std::format("Program '{}' does not exist", name));

  return it->second;
}

/* -------------------------------------------- */
/*                  PRIVATE                     */
/* -------------------------------------------- */

void ShaderManager::LoadShaders()
{
  for (auto& entry : fs::directory_iterator(GetShadersPath()))
  {
    if (fs::is_directory(entry))
      continue;

    const fs::path& entryPath = entry.path();
    String entryStr = entryPath.string();
    fs::path filenamePath = entryPath.filename();
    String filename = filenamePath.string();
    String ext = filenamePath.extension().string();

    /**
     * GL_VERTEX_SHADER
     * GL_TESS_CONTROL_SHADER
     * GL_TESS_EVALUATION_SHADER
     * GL_GEOMETRY_SHADER 
     * GL_FRAGMENT_SHADER
     */
    i32 shaderType = 0;
    if (ext == ".vert")
      shaderType = GL_VERTEX_SHADER;
    else if (ext == ".tesc")
      shaderType = GL_TESS_CONTROL_SHADER;
    else if (ext == ".tese")
      shaderType = GL_TESS_EVALUATION_SHADER;
    else if (ext == ".geom")
      shaderType = GL_GEOMETRY_SHADER;
    else if (ext == ".frag")
      shaderType = GL_FRAGMENT_SHADER;
    else
      CONSOLE_WARN("Unknown file extension {}", ext);

    IStream file(entryStr);
    if (!file)
      throw std::runtime_error(std::format("Error on opening file <{}>", entryStr));

    String source;
    source.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    Shader shader(filename);
    shader.Create(shaderType);
    shader.LoadSource(source);

    auto res = _shaders.emplace(filename, shader);
    if (!res.second)
      throw std::runtime_error(std::format("Error on loading shader <{}>", filename));
  }
}
void ShaderManager::CompileShaders()
{
  for (const auto& [key, shader] : _shaders)
  {
    CONSOLE_TRACE("Compiling {}", key);

    bool result = shader.Compile();
    if (!result)
      throw std::runtime_error(std::format("Error on compiling shader: {}", shader.GetShaderInfo()));
  }
}

void ShaderManager::LoadPrograms()
{
  ConfigFile conf((GetRootPath() / SM_FILE_CONFIG).string());
  conf.ReadData();

  for (auto const& it : conf.GetData())
  {
    const String& section = it.first; /* <- The program name */
    const String& vertex = conf.GetValue(section, "vertex");
    const String& tesc = conf.GetValue(section, "tess_control");
    const String& tese = conf.GetValue(section, "tess_eval");
    const String& geometry = conf.GetValue(section, "geometry");
    const String& fragment = conf.GetValue(section, "fragment");

    Shader* vertShader = vertex.empty()   ? nullptr : &GetShaderByName(vertex);
    Shader* tescShader = tesc.empty()     ? nullptr : &GetShaderByName(tesc);
    Shader* teseShader = tese.empty()     ? nullptr : &GetShaderByName(tese);
    Shader* geomShader = geometry.empty() ? nullptr : &GetShaderByName(geometry);
    Shader* fragShader = fragment.empty() ? nullptr : &GetShaderByName(fragment);
    
    Program program(section);
    program.Create();
    if (vertShader)  program.AttachShader(*vertShader);
    if (tescShader)  program.AttachShader(*tescShader);
    if (teseShader)  program.AttachShader(*teseShader);
    if (geomShader)  program.AttachShader(*geomShader);
    if (fragShader)  program.AttachShader(*fragShader);
    
    auto res = _programs.emplace(section, program);
    if (!res.second)
      throw std::runtime_error(std::format("Error on loading program <{}>", section));
  }
}
void ShaderManager::LinkPrograms()
{
  for (const auto& [key, program] : _programs)
  {
    CONSOLE_TRACE("Linking program <{}>", key);

    bool result = program.Link();
    if (!result)
      throw std::runtime_error(std::format("Error on linking: {}", program.GetProgramInfo()));
  }
}

void ShaderManager::SetProgramsUniforms()
{
  auto& framebufferProg = GetProgramByName("Framebuffer");
  framebufferProg.SetUniform1i("u_fboImageTexture", 0);
  framebufferProg.SetUniform1i("u_postProcessingType", 0);

  auto& sceneProg = GetProgramByName("Scene");
  sceneProg.SetUniform1i("u_material.diffuseTexture", 0);
  sceneProg.SetUniform1i("u_material.specularTexture", 1);
  sceneProg.SetUniform1i("u_material.normalTexture", 2);
  sceneProg.SetUniform1i("u_material.heightTexture", 3);
  sceneProg.SetUniform1i("u_useNormalMap", 0);

  auto& sceneShadowsProg = GetProgramByName("SceneShadows");
  sceneShadowsProg.SetUniform1i("u_material.diffuseTexture", 0);
  sceneShadowsProg.SetUniform1i("u_material.specularTexture", 1);
  sceneShadowsProg.SetUniform1i("u_material.normalTexture", 2);
  sceneShadowsProg.SetUniform1i("u_material.heightTexture", 3);
  sceneShadowsProg.SetUniform1i("u_depthMapTexture", 10);
  sceneShadowsProg.SetUniform1i("u_depthCubeMapTexture", 11);
  sceneShadowsProg.SetUniform1i("u_useNormalMap", 0);

  auto& skyboxProg = GetProgramByName("Skybox");
  skyboxProg.SetUniform1i("u_skyboxTexture", 0);
}