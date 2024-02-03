#pragma once

#include "ResourceManager.hh"
#include "../Shader.hh"

/* ------------------------------------------------
	Resource manager class class ShadersManager:
	load in and retrieve from memory shader programs 
 -------------------------------------------------- */
class ShadersManager : public ResourceManagerInterface<ShadersManager>
{
public:
	void Initialize() override;
	
	void ShutDown() override;

	/* Load in memory the shader program */
	class Shader* LoadShaderProgram(const char* label, Path vertFilePath, Path fragFilePath);
	
	/* Retrieve from memory the shader program */
	class Shader* GetShader(const char* label);

private:
	UniquePointer<Shader[]> _shaderBuffer;
	uint32_t _bufferSize;
};
