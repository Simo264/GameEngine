#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Shader.hpp"

class ShaderManager
{
public:
	ShaderManager(const ShaderManager&) = delete;
	void operator=(const ShaderManager&) = delete;

	/**
	 * @return
	 * The instance of this ShaderManager singleton class
	 */
	static ShaderManager& Get()
	{
		static ShaderManager shaderManager;
		return shaderManager;
	}

	/**
	 * @brief
	 * Load all shaders and programs
	 */
	void Initialize();

	/**
	 * @brief
	 * Destroy all shader objects and program objects
	 */
	void CleanUp();

	/**
	 * @return
	 * A reference to the mapped value of the element with specified key. 
	 * 
	 * @throw 
	 * If no such element exists, an exception of type std::out_of_range is thrown.
	 */
	Shader& GetShaderByName(StringView filename);

	/**
	 * @return
	 * A reference to the mapped value of the element with specified key.
	 * 
	 * @throw 
	 * If no such element exists, an exception of type std::out_of_range is thrown.
	 */
	Program& GetProgramByName(StringView name);

	const auto& GetShaders() const { return _shaders; }
	const auto& GetPrograms() const { return _programs; }
	
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
