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
	 * Reset uniform values of all programs
	 */
	void ResetProgramsUniforms();

	/**
	 * Get the shader object by filename
	 */
	Shader* GetShaderByName(StringView filename);

	/**
	 * Retrieve the program object by name
	 */
	Program* GetProgramByName(StringView name);
	
	constexpr Vector<Shader>& GetShadersVector() { return _shaders; }
	constexpr Vector<Program>& GetProgramsVector() { return _programs; }

private:
	ShaderManager() = default;
	~ShaderManager() = default;

	Vector<Shader>	_shaders;
	Vector<Program> _programs;

	void LoadShaderFiles();
	void LoadProgramsFromConfig();

	Shader& LoadShader(const fs::path& filepath, i32 shaderType);
	Program& LoadProgram(StringView name,
		Shader* vertex,
		Shader* tesc,
		Shader* tese,
		Shader* geometry,
		Shader* fragment
	);

};
