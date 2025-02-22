#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Shader.hpp"

/**
 * @class ShaderManager.
 * 
 * @brief Singleton class responsible for managing shader and program objects.
 * 
 * This class handles shader compilation, program linking, and cleanup.
 */
class ShadersManager
{
public:
	/** @brief Deleted copy constructor to enforce singleton pattern. */
	ShadersManager(const ShadersManager&) = delete;
	/** @brief Deleted assignment constructor to enforce singleton pattern. */
	void operator=(const ShadersManager&) = delete;

	/** Get the singleton instance of ShaderManager. */
	static ShadersManager& Get()
	{
		static ShadersManager shaderManager;
		return shaderManager;
	}

	/** @brief Load all shaders and programs. */
	void Initialize();
	/** @brief Destroy all shader objects and program objects. */
	void CleanUp();

	/** @brief Retrieve a shader by its filename. */
	Shader GetShader(StringView shaderName) const;
	Shader GetOrCreateShader(StringView shaderName);
	Shader CreateShader(StringView shaderName);

	/** @brief Retrieve a program by its name. */
	Program GetProgram(StringView programName) const;
	Program CreateProgram(StringView programName);

private:
	ShadersManager() = default;
	~ShadersManager() = default;
	
	void ReadConfig(class IniFileHandler& conf);
	i32 ResolveShaderType(StringView ext);
	void SetProgramsUniforms() const;

	// Stores all loaded shader objects.
	Vector<Shader> _shaders;
	// Each index in this vector corresponds to the same index in `_shaders`,
	// ensuring that `_shaderNames[i]` represents the name of `_shaders[i]`.
	// The names are stored as fixed-size character arrays (`char[32]`).
	Vector<Array<char, 32>> _shaderNames;

	// Stores all loaded program objects.
	Vector<Program> _programs;
	// Each index in this vector corresponds to the same index in `_programs`,
	// ensuring that `_programNames[i]` represents the name of `_programs[i]`.
	// The names are stored as fixed-size character arrays (`char[32]`).
	Vector<Array<char, 32>> _programNames;
};
