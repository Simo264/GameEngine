#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Shader.hpp"

/**
 * Load and retrieve shaders and programs 
 */
class ShaderManager
{
public:
	ShaderManager() = default;
	~ShaderManager() = default;

	/**
	 * Load all shaders from directory <dirpath>
	 */
	void LoadShadersFromDir(const fs::path& dirpath);

	/**
	 * Load all programs from configuration file SM_ProgConfig.ini
	 * 
	 */
	void LoadPrograms();


	void SetUpProgramsUniforms();

	/**
	 * Destroy all shader objects and program objects
	 */
	void CleanUp();

	/**
	 * Load the shader object in shaders array
	 */
	Shader& LoadShader(const fs::path& filepath, i32 shaderType);
	
	/**
	 * Retrieve the shader object from shaders array 
	 */
	Shader* GetShader(const char* filename);

	/**
	 * Load the program object in program array
	 * @param name: the program name
	 */
	Program& LoadProgram(const char* name,
		Shader* vertex,
		Shader* tesc,
		Shader* tese,
		Shader* geometry,
		Shader* fragment
	);

	/**
	 * Retrieve the program object from program array
	 */
	Program* GetProgram(const char* name);
	
	Vector<Shader>	shaders;
	Vector<Program> programs;
};
