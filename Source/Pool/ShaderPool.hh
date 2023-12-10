#pragma once

#include "../Core.hh"
#include "../Graphics/Shader.hh"

class ShaderPool
{
public:
	ShaderPool() = delete;
	~ShaderPool() = delete;

	static void Init();

	static std::filesystem::path GetShadersDir();
	
	static Graphics::Shader* LoadShader(
		const char* label, 
		std::filesystem::path vertFilePath,
		std::filesystem::path fragFilePath);
	
	static std::filesystem::path GetShaderFilePathByName(const char* filename);

private:
	static vector<std::filesystem::path> _shaderFilePaths;
	static array<Graphics::Shader, 10>	 _shaderBuffer;
	static uint32_t _numShaders;
};
