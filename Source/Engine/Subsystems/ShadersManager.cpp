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
  IniFileHandler conf((Filesystem::GetRootPath() / SM_FILE_CONFIG));
  conf.ReadData();

  u32 nrShaders = Utils::CountFilesInDirectory(Filesystem::GetShadersPath());
  u32 nrPrograms = conf.GetData().size();
  _shaders.reserve(nrShaders);
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

Shader ShadersManager::GetShader(StringView shaderName) const
{
	assert(shaderName.size() < 32);
  
	for (u32 i = 0; i < _shaderNames.size(); ++i)
		if (strcmp(_shaderNames[i].data(), shaderName.data()) == 0)
			return _shaders.at(i);

  return Shader{};
}
Shader ShadersManager::GetOrCreateShader(StringView shaderName)
{
	Shader s = GetShader(shaderName);
  if (!s.IsValid())
		s = CreateShader(shaderName);
  
  return s;
}
Shader ShadersManager::CreateShader(StringView shaderName)
{
	assert(shaderName.size() < 32);

  fs::path absolute = Filesystem::GetShadersPath() / shaderName.data();
  if (!fs::exists(absolute))
    throw std::runtime_error(std::format("Shader file '{}' does not exist", absolute.string()));

  IStream file(absolute);
  String shaderSrc{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
  StringView ext = shaderName.substr(shaderName.find_last_of('.') + 1);
  i32 shaderType = ResolveShaderType(ext);
  
  CONSOLE_TRACE("Create shader {}", shaderName.data());
  Shader& shader = _shaders.emplace_back();
  shader.Create(shaderType, shaderSrc);
  if (!shader.Compile())
    CONSOLE_ERROR("Error on compiling shader {}: {}", shaderName.data(), shader.GetShaderInfo());

  auto& name = _shaderNames.emplace_back();
	name.fill(0);
  std::strncpy(name.data(), shaderName.data(), shaderName.size());
  return shader;
}

Program ShadersManager::GetProgram(StringView programName) const
{
	assert(programName.size() < 32);

	for (u32 i = 0; i < _programNames.size(); ++i)
		if (strcmp(_programNames[i].data(), programName.data()) == 0)
			return _programs.at(i); 
	
  return Program{};
}
Program ShadersManager::CreateProgram(StringView programName)
{
	assert(programName.size() < 32);
  
  CONSOLE_TRACE("Create program {}", programName.data());
  Program& program = _programs.emplace_back();
  program.Create();

  auto& name = _programNames.emplace_back();
	name.fill(0);
  std::strncpy(name.data(), programName.data(), programName.size());
  return program;
}

// --------------------------------------------
//                  PRIVATE                    
// --------------------------------------------

i32 ShadersManager::ResolveShaderType(StringView ext)
{
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
  
  throw std::runtime_error(std::format("Unknown file extension {}", ext.data()));
}
void ShadersManager::ReadConfig(IniFileHandler& conf)
{
  for (auto const& it : conf.GetData())
  {
    const String& section = it.first; // The program name 
    const Program& program = CreateProgram(section);
    if (!program.IsValid())
      continue;

    const String& vertex = conf.GetValue(section, "vertex");
    const String& tesc = conf.GetValue(section, "tess_control");
    const String& tese = conf.GetValue(section, "tess_eval");
    const String& geometry = conf.GetValue(section, "geometry");
    const String& fragment = conf.GetValue(section, "fragment");
    if (!vertex.empty())
    {
      const Shader& vertShader = GetOrCreateShader(vertex);
      program.AttachShader(vertShader);
    }
    if (!tesc.empty())
    {
      const Shader& tescShader = GetOrCreateShader(tesc);
      program.AttachShader(tescShader);
    }
    if (!tese.empty())
    {
      const Shader& teseShader = GetOrCreateShader(tese);
      program.AttachShader(teseShader);
    }
    if (!geometry.empty())
    {
      const Shader& geomShader = GetOrCreateShader(geometry);
      program.AttachShader(geomShader);
    }
    if (!fragment.empty())
    {
      const Shader& fragShader = GetOrCreateShader(fragment);
      program.AttachShader(fragShader);
    }

    CONSOLE_TRACE("Link program {}", section);
    if(!program.Link())
      CONSOLE_ERROR("Error on linking program '{}': {}", section, program.GetProgramInfo());
  }
}
void ShadersManager::SetProgramsUniforms() const
{
  Program skyboxProg = GetProgram("Skybox");
  skyboxProg.SetUniform1i("u_skyboxTexture", 0);

  Program framebufferProg = GetProgram("Framebuffer");
  framebufferProg.SetUniform1i("u_fboImageTexture", 0);

  Program sceneProg = GetProgram("Scene");
  sceneProg.SetUniform1i("u_useNormalMap", 0);
  sceneProg.SetUniform1i("u_material.diffuseTexture", 0);
  sceneProg.SetUniform1i("u_material.specularTexture", 1);
  sceneProg.SetUniform1i("u_material.normalTexture", 2);

  Program sceneShadowsProg = GetProgram("SceneShadows");
  sceneShadowsProg.SetUniform1i("u_useNormalMap", 0);
  sceneShadowsProg.SetUniform1i("u_material.diffuseTexture", 0);
  sceneShadowsProg.SetUniform1i("u_material.specularTexture", 1);
  sceneShadowsProg.SetUniform1i("u_material.normalTexture", 2);
  sceneShadowsProg.SetUniform1i("u_depthMapTexture", 10);
  sceneShadowsProg.SetUniform1i("u_depthCubeMapTexture", 11);
  
  Program skeletalAnimProg = GetProgram("SkeletalAnim");
  skeletalAnimProg.SetUniform1i("u_useNormalMap", 0);
  skeletalAnimProg.SetUniform1i("u_material.diffuseTexture", 0);
  skeletalAnimProg.SetUniform1i("u_material.specularTexture", 1);
  skeletalAnimProg.SetUniform1i("u_material.normalTexture", 2);
}