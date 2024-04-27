#pragma once

#include "Core/Core.hpp"
#include "Engine/Subsystems/Manager.hpp"
#include "Engine/Graphics/Shader.hpp"


/**
 * Load and retrieve shader objects and program objects from the array
 */
class ShaderManager : public Manager<ShaderManager>
{
public:
	/**
	 * Initialize shaders array and load shaders from files
	 */
	void Initialize() override;
	
	/**
	 * Destroy all shader objects and program objects
	 */
	void CleanUp() override;

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
	
	vector<Shader> shaders;
	vector<Program> programs;
};
