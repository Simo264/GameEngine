  #include "ShadersManager.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Serialization/INIParser.hpp"
#include "Core/Serialization/JSONParser.hpp"
#include "Core/Paths/Paths.hpp"

#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Globals.hpp"
#include "Engine/Utils.hpp"
#include "Engine/Uniforms.hpp"

constexpr char SM_FILE_CONFIG[] = "ShadersConfig.json";

static i32 ResolveShaderType(StringView ext)
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

// --------------------------------------------
//                  PUBLIC                     
// --------------------------------------------

void ShadersManager::Initialize()
{
  // u32 nrShaders = Utils::CountFilesInDirectory(Paths::GetShadersPath(), true);
  // u32 nrPrograms = conf.GetData().size();
	// _shaders.reserve(nrShaders);
	// _programs.reserve(nrPrograms);

  LoadConfig((Paths::GetRootPath() / SM_FILE_CONFIG));
}
void ShadersManager::CleanUp()
{
  // Destoy all program objects
	for (auto& pair : _programs)
		pair.program.Delete();

  // Destoy all shaders objects 
	for (auto& pair : _shaders)
    pair.shader.Delete();
}

Shader ShadersManager::GetShader(StringView shaderName) const
{
	assert(shaderName.size() < 32);

  for (const auto& pair : _shaders)
  {
		StringView name = pair.name.data();
		if (name == shaderName)
			return pair.shader;
  }

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

  fs::path absolute = Paths::GetShadersPath() / shaderName.data();
  if (!fs::exists(absolute))
    throw std::runtime_error(std::format("Shader file '{}' does not exist", absolute.string()));

  IStream file(absolute);
  String shaderSrc{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
  StringView ext = shaderName.substr(shaderName.find_last_of('.') + 1);
  i32 shaderType = ResolveShaderType(ext);
  
  CONSOLE_TRACE("Create shader {}", shaderName.data());
	auto& pair = _shaders.emplace_back();
	
  Array<char, 32>& name = pair.name;
  std::copy(shaderName.begin(), shaderName.end(), name.begin());

	Shader& shader = pair.shader;
  shader.Create(shaderType, shaderSrc);
	if (!shader.Compile())
		CONSOLE_ERROR("Error on compiling shader {}: {}", shaderName.data(), pair.shader.GetShaderInfo());

 return shader;
}

Program ShadersManager::GetProgram(StringView programName) const
{
	assert(programName.size() < 32);

  for (const auto& pair : _programs)
  {
		StringView name = pair.name.data();
		if (name == programName)
			return pair.program;
  }

  return Program{};
}
Program ShadersManager::CreateProgram(StringView programName)
{
	assert(programName.size() < 32);
  
  CONSOLE_TRACE("Create program {}", programName.data());
	auto& pair = _programs.emplace_back();
	
  Array<char, 32>& name = pair.name;
  std::copy(programName.begin(), programName.end(), name.begin());
	
  Program& program = pair.program;
  program.Create();
  return program;
}

// --------------------------------------------
//                  PRIVATE                    
// --------------------------------------------

void ShadersManager::LoadConfig(const fs::path& file)
{
  nlohmann::json data = JSONParser::ParseFile(file);
	for (const auto& [programName, shadersJson] : data.items())
	{
		Program program = GetProgram(programName);
    if (program.IsValid())
    {
			CONSOLE_WARN("Program '{}' already exists", programName);
      continue;
    }

    program = CreateProgram(programName);

		if (shadersJson.contains("vertex"))
		{
			Shader shader = GetOrCreateShader(shadersJson.at("vertex"));
			program.AttachShader(shader);
		}
		if (shadersJson.contains("tess_control"))
		{
			Shader shader = GetOrCreateShader(shadersJson.at("tess_control"));
			program.AttachShader(shader);
		}
		if (shadersJson.contains("tess_eval"))
		{
			Shader shader = GetOrCreateShader(shadersJson.at("tess_eval"));
			program.AttachShader(shader);
		}
		if (shadersJson.contains("geometry"))
		{
			Shader shader = GetOrCreateShader(shadersJson.at("geometry"));
			program.AttachShader(shader);
		}
		if (shadersJson.contains("fragment"))
		{
			Shader shader = GetOrCreateShader(shadersJson.at("fragment"));
			program.AttachShader(shader);
		}
		CONSOLE_TRACE("Link program {}", programName);
		if (!program.Link())
			CONSOLE_ERROR("Error on linking program '{}': {}", programName, program.GetProgramInfo());
	}
}
