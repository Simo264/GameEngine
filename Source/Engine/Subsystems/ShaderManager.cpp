#include "ShaderManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/OpenGL.hpp"
#include "Core/FileParser/INIFileParser.hpp"

#include "Engine/Globals.hpp"

void ShaderManager::CleanUp()
{
  /* Destoy all program objects */
  for (auto& program : programs)
    program.Delete();

  /* Destoy all shaders objects */
  for (auto& shader : shaders)
    shader.Delete();

  shaders.clear();
  programs.clear();
}

void ShaderManager::LoadShadersFromDir(const fs::path& dirpath)
{
  if (!fs::exists(dirpath) || !fs::is_directory(dirpath))
  {
    CONSOLE_WARN("<dirpath> is not a valid path", dirpath.string().c_str());
    return;
  }

  /* Load all shader files in "Shaders/" directory */
  for (auto& entry : fs::directory_iterator(dirpath))
  {
    if (!fs::is_directory(entry))
    {
      string filename = entry.path().filename().string();
      uint32_t pos = filename.find_last_of('.') + 1;
      string ext = filename.substr(pos);
      fs::path path = entry.path().lexically_normal();
      CONSOLE_TRACE("Loading shader {}", path.string().c_str());

      if (ext.compare("vert") == 0)
        LoadShader(path, GL_VERTEX_SHADER);

      else if (ext.compare("tesc") == 0)
        LoadShader(path, GL_TESS_CONTROL_SHADER);
      else if (ext.compare("tese") == 0)
        LoadShader(path, GL_TESS_EVALUATION_SHADER);
      else if (ext.compare("geom") == 0)
        LoadShader(path, GL_GEOMETRY_SHADER);
      else if (ext.compare("frag") == 0)
        LoadShader(path, GL_FRAGMENT_SHADER);
      else
        CONSOLE_WARN("Error on loading shader {}: unknown .{} file extension", filename.c_str(), ext.c_str());
    }
  }
}
void ShaderManager::LoadPrograms()
{
  INIFileParser conf(ROOT_PATH / "SM_ProgConfig.ini");
  conf.ReadData();

  for (auto const& it : conf.GetData())
  {
    const string& section = it.first;
    const string vertex = conf.GetValue(section.c_str(), "vertex");
    const string tesc = conf.GetValue(section.c_str(), "tess_control");
    const string tese = conf.GetValue(section.c_str(), "tess_eval");
    const string geometry = conf.GetValue(section.c_str(), "geometry");
    const string fragment = conf.GetValue(section.c_str(), "fragment");
    Shader* vertShader = GetShader(vertex.c_str());
    Shader* tescShader = GetShader(tesc.c_str());
    Shader* teseShader = GetShader(tese.c_str());
    Shader* geomShader = GetShader(geometry.c_str());
    Shader* fragShader = GetShader(fragment.c_str());
    LoadProgram(section.c_str(), vertShader, tescShader, teseShader, geomShader, fragShader);
  }
}
void ShaderManager::SetUpProgramsUniforms()
{
  auto framebufferProg = GetProgram("Framebuffer");
  framebufferProg->SetUniform1i("u_fboImageTexture", 0);
  framebufferProg->SetUniform1i("u_postProcessingType", 0);

  auto sceneProg = GetProgram("Scene");
  sceneProg->SetUniform1i("u_material.diffuseTexture", 0);
  sceneProg->SetUniform1i("u_material.specularTexture", 1);
  sceneProg->SetUniform1i("u_material.normalTexture", 2);
  sceneProg->SetUniform1i("u_material.heightTexture", 3);
  sceneProg->SetUniform1i("u_useNormalMap", 0);

  auto sceneShadowsProg = GetProgram("SceneShadows");
  sceneShadowsProg->SetUniform1i("u_material.diffuseTexture", 0);
  sceneShadowsProg->SetUniform1i("u_material.specularTexture", 1);
  sceneShadowsProg->SetUniform1i("u_material.normalTexture", 2);
  sceneShadowsProg->SetUniform1i("u_material.heightTexture", 3);
  sceneShadowsProg->SetUniform1i("u_depthMapTexture", 10);
  sceneShadowsProg->SetUniform1i("u_depthCubeMapTexture", 11);
  sceneShadowsProg->SetUniform1i("u_useNormalMap", 0);

  auto skyboxProg = GetProgram("Skybox");
  skyboxProg->SetUniform1i("u_skyboxTexture", 0);
}

Shader& ShaderManager::LoadShader(const fs::path& filepath, int shaderType)
{
  string filepathstr = filepath.string();
  if (!fs::exists(filepath))
    CONSOLE_WARN("File {} does not exist", filepathstr);

  
  Shader& shader = shaders.emplace_back();
  shader.Create(shaderType);
  shader.filename = filepath.filename().string();

  string source;
  ifStream file(filepath);
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
Shader* ShaderManager::GetShader(const char* filename)
{
  if (!filename)
    return nullptr;

  for (auto& shader : shaders)
    if (shader.filename.compare(filename) == 0)
      return &shader;

  return nullptr;
}

Program& ShaderManager::LoadProgram(const char* name,
  Shader* vertex,
  Shader* tesc,
  Shader* tese,
  Shader* geometry,
  Shader* fragment
)
{
  Program& program = programs.emplace_back();
  program.Create();
  program.name = name;
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
Program* ShaderManager::GetProgram(const char* name)
{
  if (!name)
    return nullptr;

  for (auto& program : programs)
    if (program.name.compare(name) == 0)
      return &program;

  return nullptr;
}
