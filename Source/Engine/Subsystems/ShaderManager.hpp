#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Shader.hpp"

/**
 * Load and retrieve shaders and programs 
 */
class ShaderManager
{
public:
	ShaderManager(const ShaderManager&) = delete;
	void operator=(const ShaderManager&) = delete;

	/**
	 * Return the instance of this ShaderManager singleton class
	 */
	static ShaderManager& Get()
	{
		static ShaderManager shaderManager;
		return shaderManager;
	}

	/**
	 * Load all shaders and programs
	 */
	void Initialize();

	/**
	 * Destroy all shader objects and program objects
	 */
	void CleanUp();

	/**
	 * Get the shader object by filename
	 */
	Shader& GetShaderByName(StringView filename);

	/**
	 * Retrieve the program object by name
	 */
	Program& GetProgramByName(StringView name);

	constexpr const UnorderedMap<String, Shader>& GetShaders() const { return _shaders; }

	constexpr const UnorderedMap<String, Program>& GetPrograms() const { return _programs; }
	
private:
	ShaderManager() = default;
	~ShaderManager() = default;

	/**
	 * First: the shader file name
	 * Second: the shader object
	 */
	UnorderedMap<String, Shader> _shaders;
	
	/**
	 * First: the program name identifier
	 * Second: the program object
	 */
	UnorderedMap<String, Program> _programs;

	void LoadShaders();
	void CompileAndLoadShader(StringView pathString, StringView filename, i32 shaderType);
	
	void LoadPrograms();
	void LinkAndLoadProgram(StringView name,
		Shader* vertex,
		Shader* tesc,
		Shader* tese,
		Shader* geometry,
		Shader* fragment
	);

	void SetProgramsUniforms();
};
