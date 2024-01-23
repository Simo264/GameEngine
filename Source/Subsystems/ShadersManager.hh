#pragma once

#include "../Core.hh"
#include "../Shader.hh"

/* ------------------------------------------------
	Static class ShadersManager:
	load in and retrieve from memory shader programs 
 -------------------------------------------------- */
class ShadersManager
{
public:
	ShadersManager()	= delete;
	~ShadersManager() = delete;

	/* Load in memory the shader program */
	static Shader* LoadShaderProgram(
		const char* label, 
		Path vertFilePath,
		Path fragFilePath);
	
	/* Retrieve from memory the shader program */
	static Shader* GetShader(const char* label);
	
	static Path GetShaderFile(Path filePath);
	
	static void Initialize();
	static void ShutDown();

private:
	inline static Path _shadersDir = std::filesystem::current_path().parent_path() / "Shaders";

	inline static Array<Shader, 10> _shaderProgramsBuffer = Array<Shader, 10>();
	inline static uint32_t _nShaderPrograms = 0;
};
