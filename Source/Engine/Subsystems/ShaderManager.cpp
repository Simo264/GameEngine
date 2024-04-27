#include "ShaderManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/Platform/OpenGL/OpenGL.hpp"

static void LoadFileContent(const fspath& filepath, string& dest)
{
  ifStream file(filepath);
  if (file)
  {
    dest.assign(
      std::istreambuf_iterator<char>(file),
      std::istreambuf_iterator<char>()
    );
  }
  else
  {
    CONSOLE_WARN("Error on opening '{}' file", filepath.string().c_str());
  }
}

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void ShaderManager::Initialize()
{
  /* Load all shader files in "Shaders/" directory */
  for (auto& entry : std::filesystem::directory_iterator(SHADERS_PATH))
  {
    if (!std::filesystem::is_directory(entry))
    {
      auto filename = entry.path().filename().string();
      auto pos = filename.find_last_of('.') + 1;
      auto ext = filename.substr(pos);
      if (ext.compare("vert") == 0)
      {
        LoadShader(entry, GL_VERTEX_SHADER);
      }
      else if (ext.compare("frag") == 0)
      {
        LoadShader(entry, GL_FRAGMENT_SHADER);
      }
      else
      {
        CONSOLE_WARN("Error on loading file '{}': unknown '.{}' file extension", filename.c_str(), ext.c_str());
      }
    }
  }
}

void ShaderManager::CleanUp()
{
  /* Destoy all shaders objects */
  for (auto& shader : shaders)
    shader.Delete();

  /* Destoy all program objects */
  for (auto& program : programs)
    program.Delete();

  shaders.clear();
  programs.clear();
}

Shader& ShaderManager::LoadShader(const fspath& filepath, int shaderType)
{
  string filepathstr = filepath.lexically_normal().string();

  Shader& shader = shaders.emplace_back();
  shader.Create(shaderType);

  shader.filename = filepath.filename().string();

  string filecontent;
  LoadFileContent(filepathstr.c_str(), filecontent);

  shader.LoadSource(filecontent.c_str(), filecontent.size());
  bool compiled = shader.Compile();
  if (!compiled)
  {
    CONSOLE_WARN("Error on compiling shader '{}': {}", filepathstr.c_str(), shader.GetShaderInfo());
  }

  return shader;
}

Shader* ShaderManager::GetShader(const char* filename)
{
  for (auto& shader : shaders)
  {
    if (shader.filename.compare(filename) == 0)
      return &shader;
  }

  return nullptr;
}

Program& ShaderManager::LoadProgram(const char* programName, Shader& vertexShader, Shader& fragmentShader)
{
  Program& program = programs.emplace_back();
  program.Create();
  program.name = programName;
  program.AttachShader(vertexShader);
  program.AttachShader(fragmentShader);
  bool link = program.Link();
  if (!link)
  {
    CONSOLE_WARN("Error on linking program {}: {}", programName, program.GetProgramInfo());
  }

  return program;
}

Program* ShaderManager::GetProgram(const char* name)
{
  for (auto& program : programs)
  {
    if (program.name.compare(name) == 0)
      return &program;
  }

  return nullptr;
}
