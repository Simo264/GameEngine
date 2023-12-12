#pragma once

#include "../Core.hh"
#include "../Graphics/Shader.hh"

class ShaderPool
{
public:
	ShaderPool() = delete;
	~ShaderPool() = delete;

	static void Init();

	static Graphics::Shader* LoadShader(
		const char* label, 
		std::filesystem::path vertFilePath,
		std::filesystem::path fragFilePath);
	
	static Graphics::Shader* GetShaderByLabel(const string& label);

	static std::filesystem::path GetShaderFileByName(const char* filename);

private:
	static vector<std::filesystem::path> _shaderFiles; // all *.vert *.frag files
	static array<Graphics::Shader, 10>	 _shaderProgramsBuffer;
	static uint32_t _nShaderPrograms; 
};
