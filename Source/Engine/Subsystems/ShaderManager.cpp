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
  LoadShaderFiles();

  /* Load programs */
  LoadProgramsFromConfig();
}

void ShaderManager::CleanUp()
{
  /* Destoy all program objects */
  for (auto& program : _programs)
    program.Delete();

  /* Destoy all shaders objects */
  for (auto& shader : _shaders)
    shader.Delete();

  Vector<Shader>().swap(_shaders);
  Vector<Program>().swap(_programs);
}

void ShaderManager::ResetProgramsUniforms()
{
  auto framebufferProg = GetProgramByName("Framebuffer");
  framebufferProg->SetUniform1i("u_fboImageTexture", 0);
  framebufferProg->SetUniform1i("u_postProcessingType", 0);

  auto sceneProg = GetProgramByName("Scene");
  sceneProg->SetUniform1i("u_material.diffuseTexture", 0);
  sceneProg->SetUniform1i("u_material.specularTexture", 1);
  sceneProg->SetUniform1i("u_material.normalTexture", 2);
  sceneProg->SetUniform1i("u_material.heightTexture", 3);
  sceneProg->SetUniform1i("u_useNormalMap", 0);

  auto sceneShadowsProg = GetProgramByName("SceneShadows");
  sceneShadowsProg->SetUniform1i("u_material.diffuseTexture", 0);
  sceneShadowsProg->SetUniform1i("u_material.specularTexture", 1);
  sceneShadowsProg->SetUniform1i("u_material.normalTexture", 2);
  sceneShadowsProg->SetUniform1i("u_material.heightTexture", 3);
  sceneShadowsProg->SetUniform1i("u_depthMapTexture", 10);
  sceneShadowsProg->SetUniform1i("u_depthCubeMapTexture", 11);
  sceneShadowsProg->SetUniform1i("u_useNormalMap", 0);

  auto skyboxProg = GetProgramByName("Skybox");
  skyboxProg->SetUniform1i("u_skyboxTexture", 0);
}

Shader* ShaderManager::GetShaderByName(StringView filename)
{
  if (filename.empty())
    return nullptr;

  for (auto& shader : _shaders)
    if (shader.filename == filename)
      return &shader;

  return nullptr;
}

Program* ShaderManager::GetProgramByName(StringView name)
{
  if (name.empty())
    return nullptr;

  for (auto& program : _programs)
    if (program.name == name)
      return &program;

  return nullptr;
}

/* -------------------------------------------- */
/*                  PRIVATE                     */
/* -------------------------------------------- */

void ShaderManager::LoadShaderFiles()
{
  for (auto& entry : fs::directory_iterator(SHADERS_PATH))
  {
    if (fs::is_directory(entry))
      continue;

    const fs::path& entryPath = entry.path();
    const fs::path entryPathFilename = entryPath.filename();
    const fs::path entryPathFilenameExt = entryPathFilename.extension();
    const String filenameString = entryPathFilename.string();
    CONSOLE_TRACE("Loading shader {}", filenameString.c_str());

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
      CONSOLE_WARN("Error on loading shader {}: unknown file extension", filenameString);
  }
}
Shader& ShaderManager::LoadShader(const fs::path& filepath, i32 shaderType)
{
  String filepathstr = filepath.string();
  if (!fs::exists(filepath))
    CONSOLE_WARN("File {} does not exist", filepathstr);

  Shader& shader = _shaders.emplace_back();
  shader.Create(shaderType);
  shader.filename = filepath.filename().string();

  String source;
  IStream file(filepath);
  if (file)
    source.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
  else
    CONSOLE_WARN("Error on opening file {}", filepathstr);

  shader.LoadSource(source);
  bool compiled = shader.Compile();
  if (!compiled)
    CONSOLE_WARN("Error on compiling shader {}: {}", filepathstr, shader.GetShaderInfo());

  return shader;
}

void ShaderManager::LoadProgramsFromConfig()
{
  ConfigFile conf(ROOT_PATH / SM_FILE_CONFIG);
  conf.ReadData();

  for (auto const& it : conf.GetData())
  {
    const String& section = it.first;
    const String& vertex = conf.GetValue(section, "vertex");
    const String& tesc = conf.GetValue(section, "tess_control");
    const String& tese = conf.GetValue(section, "tess_eval");
    const String& geometry = conf.GetValue(section, "geometry");
    const String& fragment = conf.GetValue(section, "fragment");

    Shader* vertShader = GetShaderByName(vertex);
    Shader* tescShader = GetShaderByName(tesc);
    Shader* teseShader = GetShaderByName(tese);
    Shader* geomShader = GetShaderByName(geometry);
    Shader* fragShader = GetShaderByName(fragment);
    LoadProgram(section, vertShader, tescShader, teseShader, geomShader, fragShader);
  }
}
Program& ShaderManager::LoadProgram(StringView name,
  Shader* vertex,
  Shader* tesc,
  Shader* tese,
  Shader* geometry,
  Shader* fragment
)
{
  Program& program = _programs.emplace_back();
  program.Create();
  program.name = name.data();
  if (vertex)    program.AttachShader(*vertex);
  if (tesc)      program.AttachShader(*tesc);
  if (tese)      program.AttachShader(*tese);
  if (geometry)  program.AttachShader(*geometry);
  if (fragment)  program.AttachShader(*fragment);

  bool link = program.Link();
  if (!link)
    CONSOLE_WARN("Error on linking program {}: {}", name, program.GetProgramInfo());

  return program;
}

