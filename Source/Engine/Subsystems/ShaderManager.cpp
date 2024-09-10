#include "ShaderManager.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Filesystem/ConfigFile.hpp"

#include "Engine/Globals.hpp"

constexpr StringView SM_FILE_CONFIG{ "Shader_Manager.ini" };

/* -------------------------------------------- */
/*                  PUBLIC                      */
/* -------------------------------------------- */

void ShaderManager::Initialize()
{
  /* Load shader files */
  CONSOLE_INFO("Loading shaders...");
  LoadShaderFiles();

  /* Load programs */
  CONSOLE_INFO("Loading programs...");
  LoadProgramsFromConfig();

  ResetProgramsUniforms();
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
void ShaderManager::ResetProgramsUniforms()
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
Shader& ShaderManager::GetShaderByName(StringView filename)
{
  const auto& it = _shaders.find(filename.data());
  if (it == _shaders.end())
    throw std::runtime_error(std::format("Shader '{}' does not exist", filename.data()));
  
  return it->second;
}
Program& ShaderManager::GetProgramByName(StringView name)
{
  const auto& it = _programs.find(name.data());
  if (it == _programs.end())
    throw std::runtime_error(std::format("Program '{}' does not exist", name.data()));

  return it->second;
}

/* -------------------------------------------- */
/*                  PRIVATE                     */
/* -------------------------------------------- */

void ShaderManager::LoadShaderFiles()
{
  for (auto& entry : fs::directory_iterator(GetShadersPath()))
  {
    if (fs::is_directory(entry))
      continue;

    const fs::path& entryPath = entry.path();
    const fs::path entryPathFilenameExt = entryPath.filename().extension();

    if (entryPathFilenameExt == ".vert")
      LoadShader(entryPath, GL_VERTEX_SHADER);
    else if (entryPathFilenameExt == ".tesc")
      LoadShader(entryPath, GL_TESS_CONTROL_SHADER);
    else if (entryPathFilenameExt == ".tese")
      LoadShader(entryPath, GL_TESS_EVALUATION_SHADER);
    else if (entryPathFilenameExt == ".geom")
      LoadShader(entryPath, GL_GEOMETRY_SHADER);
    else if (entryPathFilenameExt == ".frag")
      LoadShader(entryPath, GL_FRAGMENT_SHADER);
    else
      CONSOLE_WARN("Unknown file extension {}", entryPathFilenameExt.string());
  }
}
void ShaderManager::LoadShader(const fs::path& filepath, i32 shaderType)
{
  const String filenameString = filepath.filename().string();

  Shader shader;
  shader.Create(shaderType);
  shader.filename = filenameString;

  IStream file(filepath);
  if (!file.is_open())
  {
    CONSOLE_CRITICAL("Error on opening file <{}>", filenameString);
    throw std::runtime_error("Error on opening file");
  }

  String source;
  source.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
  shader.LoadSource(source);
  file.close();
  
  bool compiled = shader.Compile();
  if (!compiled)
  {
    CONSOLE_CRITICAL("Error on compiling shader <{}>", filenameString);
    throw std::runtime_error("Error on compiling shader");
  }

  auto res = _shaders.insert({ shader.filename, shader });
  if (res.second)
    CONSOLE_TRACE("<{}> has been loaded successfully", filenameString);
  else
    CONSOLE_WARN("Error on loading shader <{}>", filenameString);
}
void ShaderManager::LoadProgramsFromConfig()
{
  ConfigFile conf(GetRootPath() / SM_FILE_CONFIG);
  conf.ReadData();

  for (auto const& it : conf.GetData())
  {
    const String& section = it.first;
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
    LoadProgram(section, vertShader, tescShader, teseShader, geomShader, fragShader);
  }
}
void ShaderManager::LoadProgram(StringView name,
  Shader* vertex,
  Shader* tesc,
  Shader* tese,
  Shader* geometry,
  Shader* fragment
)
{
  Program program;
  program.Create();
  program.name = name.data();
  if (vertex)    program.AttachShader(*vertex);
  if (tesc)      program.AttachShader(*tesc);
  if (tese)      program.AttachShader(*tese);
  if (geometry)  program.AttachShader(*geometry);
  if (fragment)  program.AttachShader(*fragment);

  bool link = program.Link();
  if (!link)
  {
    CONSOLE_CRITICAL("Error on linking program <{}>: {}", name, program.GetProgramInfo());
    throw std::runtime_error("Error on linking program");
  }

  auto res = _programs.insert({ program.name, program });
  if (res.second)
    CONSOLE_TRACE("<{}> has been loaded successfully", name);
  else
    CONSOLE_WARN("Error on loading program <{}>", name);
}

