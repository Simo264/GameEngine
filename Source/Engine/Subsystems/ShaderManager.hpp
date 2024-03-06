#pragma once

#include "Core/Core.hpp"
#include "Engine/Subsystems/Manager.hpp"

/* ------------------------------------------------
	ShaderManager subsystem:
	load and retrieve from memory shader programs 
 -------------------------------------------------- */
class ShaderManager : public Manager<ShaderManager>
{
public:
	/* Initialize shaders vector */
	void Initialize() override;
	
	/* Destroy all shader objects and destroy vector */
	void CleanUp() override;

	/* Load in memory the shader program */
	class Shader& LoadShader(const char* label, const Path& vertFilePath, const Path& fragFilePath);
	
	/* Retrieve from memory the shader program */
	class Shader& GetShader(const char* label) const;

private:
	Vector<class Shader*> _shaders;
};
