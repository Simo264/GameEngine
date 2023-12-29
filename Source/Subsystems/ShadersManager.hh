#pragma once

#include "../Core.hh"
#include "../Graphics/Shader.hh"

// Load and retrieve shader files located within the Shaders directory
// ------------------------------------------------------------------------
class ShadersManager
{
public:
	ShadersManager()	= delete;
	~ShadersManager() = delete;

	static void Initialize();

	static Shader* LoadShaderProgram(
		const char* label, 
		std::filesystem::path vertFilePath,
		std::filesystem::path fragFilePath);
	
	static Shader* GetShader(const char* label);

	static std::filesystem::path GetShaderFile(std::filesystem::path filePath);

private:
	static std::filesystem::path _shadersDir;

	static array<Shader, 10> _shaderProgramsBuffer;
	static uint32_t _nShaderPrograms;
};
