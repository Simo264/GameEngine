#pragma once

#include "Engine/Subsystems/ResourceManager.hpp"
#include "Engine/Shader.hpp"

/* ------------------------------------------------
	Resource manager class class ShadersManager:
	load in and retrieve from memory shader programs 
 -------------------------------------------------- */
class ShadersManager : public ResourceManagerInterface<ShadersManager>
{
public:
	/* Initialize shader buffer */
	void Initialize() override;
	
	/* Destroy all shader objects and free buffer */
	void ShutDown() override;

	/* Load in memory the shader program */
	class Shader* LoadShaderProgram(const char* label, Path vertFilePath, Path fragFilePath);
	
	/* Retrieve from memory the shader program */
	class Shader* GetShader(const char* label);

private:
	UniquePointer<Shader[]> _shaderBuffer;
	uint32_t _bufferSize;
};
