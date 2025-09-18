#pragma once

#include "Core/Core.hpp"
#include "Core/DesignPatterns/Singleton.hpp"
#include "Engine/Graphics/Shader.hpp"

/**
 * @brief Singleton class responsible for managing shader and program objects.
 * 
 * This class handles shader compilation, program linking, and cleanup.
 */
class ShadersManager : public Singleton<ShadersManager>
{
public:
	/** @brief Load all shaders and programs. */
	void Initialize();
	/** @brief Destroy all shader objects and program objects. */
	void Cleanup();

	/** @brief Retrieve a shader by its filename. */
	Shader GetShader(StringView shaderName) const;
	Shader CreateShader(StringView shaderName, ShaderType type);
	Shader GetOrCreateShader(StringView shaderName, ShaderType type);

	/** @brief Retrieve a program by its name. */
	Program GetProgram(StringView programName) const;
	Program CreateProgram(StringView programName);

private:
	void __LoadConfig(const fs::path& path);

	struct __PairShaderName
	{
		Shader shader;					// Shader object
		Array<char, 32> name{}; // Shader name
	};
	struct __PairProgramName
	{
		Program program;				// Program object
		Array<char, 32> name{}; // Program name
	};

	// Stores all loaded shaders
	Vector<__PairShaderName> _shaders;
	// Stores all loaded programs
	Vector<__PairProgramName> _programs;
};
