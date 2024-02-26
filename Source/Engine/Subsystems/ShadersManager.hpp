#pragma once

#include "Core/Core.hpp"
#include "Engine/Subsystems/ResourceManager.hpp"

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
	class Shader* GetShader(const char* label) const;

	/* Return pointer to first shader object in buffer */
	class Shader* Begin() const;

	/* Return pointer to last shader object in buffer */
	class Shader* End() const;

private:
	class Shader* _shaderBuffer;
	uint32_t _bufferSize;
};
