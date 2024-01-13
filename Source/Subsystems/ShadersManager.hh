#pragma once

#include "../Core.hh"
#include "../Graphics/Shader.hh"

// Static class ShadersManager: 
// load and retrieve shader programs located within the Shaders directory
// ----------------------------------------------------------------------
class ShadersManager
{
public:
	ShadersManager()	= delete;
	~ShadersManager() = delete;

	static Shader* LoadShaderProgram(
		const char* label, 
		Path vertFilePath,
		Path fragFilePath);
	
	static Shader* GetShader(const char* label);
	static Path GetShaderFile(Path filePath);
	
	static void ShutDown();

private:
	inline static Path _shadersDir = std::filesystem::current_path().parent_path() / "Shaders";

	inline static Array<Shader, 10> _shaderProgramsBuffer = Array<Shader, 10>();
	inline static uint32_t _nShaderPrograms = 0;
};
