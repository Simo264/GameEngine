#pragma once

#include "../Core.hh"
#include "../Graphics/Shader.hh"

// Load and retrieve shader files located within the Shaders directory
// ------------------------------------------------------------------------
class ShadersManager
{
public:
	ShadersManager() = delete;
	~ShadersManager() = delete;

	static void Init();

	static Graphics::Shader* LoadShaderProgram(
		const char* label, 
		std::filesystem::path vertFilePath,
		std::filesystem::path fragFilePath);
	
	static Graphics::Shader* GetShaderProgramByLabel(const string& label);

	static std::filesystem::path GetShaderFile(std::filesystem::path filePath);

private:
	static std::filesystem::path _shadersDir;

	static array<Graphics::Shader, 10> _shaderProgramsBuffer;
	static uint32_t _nShaderPrograms;
};
