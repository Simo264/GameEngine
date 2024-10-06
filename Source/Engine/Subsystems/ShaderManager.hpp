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
	 * Returns a reference to the mapped value of the element with specified key. 
	 * If no such element exists, an exception of type std::out_of_range is thrown.
	 */
	Shader& GetShaderByName(StringView filename);

	/**
	 * Returns a reference to the mapped value of the element with specified key. 
	 * If no such element exists, an exception of type std::out_of_range is thrown.
	 */
	Program& GetProgramByName(StringView name);

	const UnorderedMap<String, Shader>& GetShaders() const { return _shaders; }
	
	const UnorderedMap<String, Program>& GetPrograms() const { return _programs; }
	
private:
	ShaderManager() = default;
	~ShaderManager() = default;

	/**
	 * First: the name of the shader file 
	 * Second: the shader object
	 */
	UnorderedMap<String, Shader> _shaders;
	
	/**
	 * First: the name of the program  
	 * Second: the program object
	 */
	UnorderedMap<String, Program> _programs;

	void LoadShaders();
	void CompileShaders();
	
	void LoadPrograms();
	void LinkPrograms();

	void SetProgramsUniforms();
};
