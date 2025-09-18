#include "ShadersManager.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Serialization/JSONParser.hpp"
#include "Core/Paths/Paths.hpp"

#include "Engine/Vertex.hpp"
#include "Engine/Globals.hpp"
#include "Engine/Utils.hpp"
#include "Engine/Uniforms.hpp"

constexpr auto SM_FILE_CONFIG = "ShadersConfig.json";

// --------------------------------------------
//                  PUBLIC
// --------------------------------------------

void ShadersManager::Initialize()
{
	auto p = (Paths::GetRootPath() / SM_FILE_CONFIG);
	if (!fs::exists(p))
		throw std::runtime_error(std::format("{} does not exist!", p.string()));

	__LoadConfig(p);
}
void ShadersManager::Cleanup()
{
	// Destoy all program objects
	for (auto &pair : _programs)
		pair.program.Delete();

	// Destoy all shaders objects
	for (auto &pair : _shaders)
		pair.shader.Delete();
}

Shader ShadersManager::GetShader(StringView shaderName) const
{
	assert(shaderName.size() < 32);

	for (const auto& pair : _shaders)
	{
		auto name = StringView(pair.name.data());
		if (name == shaderName)
			return pair.shader;
	}

	return Shader{};
}
Shader ShadersManager::GetOrCreateShader(StringView shaderName, ShaderType type)
{
	auto s = GetShader(shaderName);
	if (!s.IsValid())
		s = CreateShader(shaderName, type);

	return s;
}
Shader ShadersManager::CreateShader(StringView shaderName, ShaderType type)
{
	assert(shaderName.size() < 32);

	auto absolute = Paths::GetShadersPath() / shaderName.data();
	if (!fs::exists(absolute))
		throw std::runtime_error(std::format("Shader file '{}' does not exist", absolute.string()));

	auto& pairShaderName = _shaders.emplace_back();
	auto& name = pairShaderName.name;
	std::copy(shaderName.begin(), shaderName.end(), name.begin());

	auto file = InputFileStream(absolute);
	auto shaderSrc = String(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

	auto& shaderObj = pairShaderName.shader;
	shaderObj.Create(type, shaderSrc);
	if (!shaderObj.Compile())
		CONSOLE_ERROR("Error on compiling shader {}: {}", shaderName.data(), shaderObj.GetShaderInfo());

	return shaderObj;
}

Program ShadersManager::GetProgram(StringView programName) const
{
	assert(programName.size() < 32);

	for (const auto &pair : _programs)
	{
		auto name = StringView(pair.name.data());
		if (name == programName)
			return pair.program;
	}
	return Program{};
}
Program ShadersManager::CreateProgram(StringView programName)
{
	assert(programName.size() < 32);

	auto& pairProgramName = _programs.emplace_back();
	auto& name = pairProgramName.name;
	std::copy(programName.begin(), programName.end(), name.begin());

	auto& programObj = pairProgramName.program;
	programObj.Create();
	return programObj;
}

// --------------------------------------------
//                  PRIVATE
// --------------------------------------------

void ShadersManager::__LoadConfig(const fs::path &path)
{
	auto file = InputFileStream(path);
	auto data = nlohmann::json::parse(file);
	for (const auto& [programName, shadersJson] : data.items())
	{
		auto program = GetProgram(programName);
		if (program.IsValid())
		{
			CONSOLE_WARN("Program '{}' already exists", programName);
			continue;
		}

		program = CreateProgram(programName);
		
		if (shadersJson.contains("vertex"))
		{
			auto shader = GetOrCreateShader(shadersJson.at("vertex").get<String>(), ShaderType::Vertex);
			program.AttachShader(shader);
		}
		if (shadersJson.contains("tess_control"))
		{
			auto shader = GetOrCreateShader(shadersJson.at("tess_control").get<String>(), ShaderType::TessControl);
			program.AttachShader(shader);
		}
		if (shadersJson.contains("tess_eval"))
		{
			auto shader = GetOrCreateShader(shadersJson.at("tess_eval").get<String>(), ShaderType::TessEvaluation);
			program.AttachShader(shader);
		}
		if (shadersJson.contains("geometry"))
		{
			auto shader = GetOrCreateShader(shadersJson.at("geometry").get<String>(), ShaderType::Geometry);
			program.AttachShader(shader);
		}
		if (shadersJson.contains("fragment"))
		{
			auto shader = GetOrCreateShader(shadersJson.at("fragment").get<String>(), ShaderType::Fragment);
			program.AttachShader(shader);
		}
		if (!program.Link())
			throw std::runtime_error(std::format("Error on linking program '{}': {}", programName, program.GetProgramInfo()));
	}
}
