#include "ShaderManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/OpenGL.hpp"
#include "Core/FileParser/INIFileParser.hpp"

extern float Z_NEAR;
extern float Z_FAR;

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

void ShaderManager::LoadShadersFromDir(const fspath& dirpath)
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
      fspath path = entry.path().lexically_normal();

      if (ext.compare("vert") == 0)
        LoadShader(path, GL_VERTEX_SHADER);
      else if (ext.compare("frag") == 0)
        LoadShader(path, GL_FRAGMENT_SHADER);
      else if (ext.compare("geom") == 0)
        LoadShader(path, GL_GEOMETRY_SHADER);
      else
        CONSOLE_WARN("Error on loading file '{}': unknown '.{}' file extension", filename.c_str(), ext.c_str());
    }
  }
}
void ShaderManager::LoadPrograms()
{
  INIFileParser conf(ROOT_PATH / "SM_ProgConfig.ini");
  conf.ReadData();

  for (auto const& it : conf.GetData())
  {
    const string& section = it.first; /* section = program name */
    const string vertex   = conf.GetValue(section.c_str(), "vertex");
    const string geometry = conf.GetValue(section.c_str(), "geometry");
    const string fragment = conf.GetValue(section.c_str(), "fragment");
    Shader* vs = GetShader(vertex.c_str());
    Shader* gs = GetShader(geometry.c_str());
    Shader* fs = GetShader(fragment.c_str());
    LoadProgram(section.c_str(), vs, fs, gs);
  }
}
void ShaderManager::SetUpProgramsUniforms()
{
  auto framebufferProg = GetProgram("Framebuffer");
  framebufferProg->SetUniform1i("u_screenTexture", 0);
  framebufferProg->SetUniform1i("u_postProcessingType", 0);

  auto sceneProg = GetProgram("Scene");
  sceneProg->SetUniform1i("u_material.diffuse", 0);
  sceneProg->SetUniform1i("u_material.specular", 1);
  sceneProg->SetUniform1f("u_material.shininess", 32.0f);
  sceneProg->SetUniform1f("u_gamma", 2.2f);

  auto shadowMapProg = GetProgram("ShadowMap");
  shadowMapProg->SetUniform1i("u_material.diffuse", 0);
  shadowMapProg->SetUniform1i("u_material.specular", 1);
  shadowMapProg->SetUniform1f("u_material.shininess", 32.0f);
  shadowMapProg->SetUniform1i("u_shadowMap", 10);
  shadowMapProg->SetUniform1f("u_gamma", 2.2f);

  auto visualShadowDepthProg = GetProgram("VisualShadowDepth");
  visualShadowDepthProg->SetUniform1i("u_depthMap", 0);
  visualShadowDepthProg->SetUniform1f("u_nearPlane", Z_NEAR);
  visualShadowDepthProg->SetUniform1f("u_farPlane", Z_FAR);

  auto skyboxProg = GetProgram("Skybox");
  skyboxProg->SetUniform1i("u_skybox", 0);
}

Shader& ShaderManager::LoadShader(const fspath& filepath, int shaderType)
{
  if (!fs::exists(filepath))
    CONSOLE_WARN("File '{}' does not exist", filepath.string().c_str());

  string filepathstr = filepath.string();
  string filename = filepath.filename().string();

  Shader& shader = shaders.emplace_back();
  shader.Create(shaderType);
  shader.filename = filename;

  string filecontent;
  ifStream file(filepath);
  if (file)
    filecontent.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
  else
    CONSOLE_WARN("Error on opening '{}' file", filepath.string().c_str());

  shader.LoadSource(filecontent.c_str(), filecontent.size());
  bool compiled = shader.Compile();
  if (!compiled)
    CONSOLE_WARN("Error on compiling shader '{}': {}", filepathstr.c_str(), shader.GetShaderInfo());

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

Program& ShaderManager::LoadProgram(const char* name, Shader* vertex, Shader* geometry, Shader* fragment)
{
  Program& program = programs.emplace_back();
  program.Create();
  program.name = name;
  if(vertex) program.AttachShader(*vertex);
  if(geometry) program.AttachShader(*geometry);
  if(fragment) program.AttachShader(*fragment);

  bool link = program.Link();
  if (!link)
    CONSOLE_WARN("Error on linking program {}: {}", name, program.GetProgramInfo());

  return program;
}
Program* ShaderManager::GetProgram(const char* name)
{
  for (auto& program : programs)
    if (program.name.compare(name) == 0)
      return &program;

  return nullptr;
}
