#pragma once

#include "Core/Core.hpp"
#include "Engine/Subsystems/Manager.hpp"

/**
 * Load and retrieve shader program objects from memory 
 */
class ShaderManager : public Manager<ShaderManager>
{
public:
	/**
	 * Initialize shader buffer
	 */
	void Initialize() override;
	
	/**
	 * Destroy all shader objects and free buffer
	 */
	void CleanUp() override;

	/**
	 * Load the shader program in memory 
	 */
	class Shader& LoadShader(const char* label, const Path& vertFilePath, const Path& fragFilePath);
	
	/**
	 * Retrieve the shader program from memory 
	 */
	class Shader& GetShader(const char* label) const;

private:
	Vector<class Shader*> _shaders;
};
