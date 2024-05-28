#pragma once

#include "Core/Core.hpp"
#include "Engine/Subsystems/Manager.hpp"
#include "Engine/Graphics/Shader.hpp"

/**
 * Load and retrieve shaders and programs 
 */
class ShaderManager : public Manager<ShaderManager>
{
public:
	/**
	 * Destroy all shader objects and program objects
	 */
	void CleanUp();

	/**
	 * Load the shader object in shaders array
	 */
	Shader& LoadShader(const fspath& filepath, int shaderType);
	
	/**
	 * Retrieve the shader object from shaders array 
	 */
	Shader* GetShader(const char* filename);

	/**
	 * Load the shader object in shaders array
	 */
	Program& LoadProgram(const char* programName, Shader& vertexShader, Shader& fragmentShader);

	/**
	 * Retrieve the shader object from shaders array
	 */
	Program* GetProgram(const char* name);
	
	vector<Shader>	shaders;
	vector<Program> programs;
};
